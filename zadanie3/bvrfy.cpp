#include <iostream>
#include <fstream>
#include <gmpxx.h>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

// COMPILATION:
// g++ --std=c++11 -o bvrfy bvrfy.cpp -lgmpxx -lgmp

using namespace std;


unsigned char conv_to_char(char r){

    if(r>='a')
        return r-'a'+10;
    else
        return r-'0';
}
long timestamp(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long ms = tp.tv_sec * 1000000 + tp.tv_usec;
    return ms;
}
void checksign(string se,string sn,string sk,string ss){

    long ts = timestamp();
    mpz_t e,n,k;
    mpz_init_set_str(e,se.c_str(),16);
    mpz_init_set_str(n,sn.c_str(),16);
    mpz_init_set_str(k,sk.c_str(),16);
    if(mpz_cmp(k,n)>0){
        cout<<"Wiadomość zbyt duża!"<<endl;
    } else {
        size_t len = 16384;
        mpz_powm(k,k,e,n);
        char chartable[len*2];
        for(int f = 0;f<len*2;f++){
            chartable[f]=0;
        }
        gmp_sprintf(chartable,"%Zx",k);
        int f = 0;
        while(f<len && chartable[f*2]!=0){
            unsigned char z1 = conv_to_char(chartable[f*2]);
            unsigned char z2 = conv_to_char(chartable[f*2+1]);
            chartable[f]=(char)(z1*16+z2);
            f++;
        }
        chartable[f]=0;
        cout<<string(chartable)<<endl<<ss<<endl;
        if(string(chartable)==ss){
            cout<<"Podpis jest zgodny z wiadomością"<<endl;
        } else {
            cout<<"Podpis nie jest zgodny z wiadomością"<<endl;
        }
        ts = timestamp()-ts;
        cout<<"Sprawdzanie podpisu zajęło "<<((double)ts)/1000000<<" s."<<endl;
    }
}

char comm[] = "Sposób działania programu: \n-bvrfy [PLIK Z KLUCZEM PUBLICZNYM] [PLIK Z WIADOMOŚCIĄ] [PLIK Z PODPISEM]";
int main(int argc,char* argv[]){
    if(argc==4){
        string pub = string(argv[1]);
        string mes = string(argv[2]);
        string sign = string(argv[3]);
        ifstream fpub(pub),fmes(mes),fsign(sign);
        if(!fpub.is_open()){
            cout<<"Plik z kluczem niewczytany. Błąd!"<<endl;
        } else {
            if(!fmes.is_open()){
                cout<<"Plik z wiadomością niewczytany. Błąd!"<<endl;
            } else {
                if(!fsign.is_open()){
                    cout<<"Plik z podpisem niewczytany. Błąd!"<<endl;
                } else {
                    string e = "",n = "",m = "",s = "";
                    getline(fpub,e);
                    getline(fpub,n);
                    fpub.close();
                    getline(fmes,m,'\0');
                    fmes.close();
                    getline(fsign,s);
                    fsign.close();
                    checksign(e,n,s,m);
                }
            }
        }
    } else {
        cout << comm<<endl;
    }
}