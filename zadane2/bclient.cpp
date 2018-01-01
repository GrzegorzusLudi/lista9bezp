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
// g++ --std=c++11 -o bclient bclient.cpp -lgmpxx -lgmp

using namespace std;


unsigned char halfbyte(unsigned char r){

    if(r<10)
        return '0'+r;
    else
        return 'a'+(r-10);
}
size_t len = 16384;
string runsocket(int port,mpz_t random,mpz_t mprim,mpz_t m,string sm,mpz_t n){

    int keysize = sm.length()*2;
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[len] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
    }
    char line[len];
    gmp_sprintf (line,"%Zx", mprim);
    send(sock , line , len , 0 );
    valread = read( sock , buffer, len);
    return string(buffer);
}
long timestamp(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long ms = tp.tv_sec * 1000000 + tp.tv_usec;
    return ms;
}
void generatemprim(int port,string se,string sn,string sm,string mes){

    long ts = timestamp();
    mpz_t random,e,n,m,gcda,mprim,randome,s,randomr;
    gmp_randstate_t rs;
    int sl = sm.length();
    char ascii[sl*2+2];
    for(int i=0;i<sl;i++){
        unsigned char c = sm[i];
        //cout<<halfbyte(c/8)<<halfbyte(c%8);
        ascii[i*2] = halfbyte(c/16);
        ascii[i*2+1] = halfbyte(c%16);
        ascii[i*2+2] = 0;
    }
    cout<<ascii<<endl;
    mpz_init(gcda);
    mpz_init(random);
    //mpz_init(s);
    mpz_init(randomr);
    mpz_init_set_str(m,ascii,16);
    mpz_init_set(mprim,m);
    mpz_init_set_str(e,se.c_str(),16);
    mpz_init_set_str(n,sn.c_str(),16);
    gmp_randinit_default(rs);
    gmp_randseed_ui (rs, (unsigned int)time(0));
    do{
        mpz_urandomm(random,rs,n);
        mpz_gcd(gcda,n,random);
    } while(mpz_cmp_ui(gcda,1)!=0);
    if(mpz_cmp(m,n)>0){
        cout<<"Wiadomość zbyt duża!"<<endl;
    } else {
        mpz_init_set(randome,random);
        mpz_powm(randome,randome,e,n);
        mpz_mul(mprim,mprim,randome);
        mpz_mod(mprim,mprim,n);

        string response = runsocket(port,random,mprim,m,sm,n);
        //cout<< response <<endl;
        mpz_init_set_str(s,response.c_str(),16);
        mpz_invert(randomr,random,n);
        mpz_mul(s,s,randomr);
        mpz_mod(s,s,n);
        char podpis[len];
        gmp_sprintf (podpis,"%Zx", s);
        cout<<"Podpis: "<<podpis<<endl<<"Zapisano do pliku \""<<mes<<".podpis\""<<endl;
        ofstream pdp(mes+".podpis");
        pdp<<podpis;
        pdp.close();
        ts = timestamp()-ts;
        cout<<"Podpisywanie wiadomości zajęło "<<((double)ts)/1000000<<" s."<<endl;
    }
}

char comm[] = "Sposób działania programu: \n-bclient [PORT] [PLIK Z KLUCZEM PUBLICZNYM] [PLIK Z WIADOMOŚCIĄ]";
int main(int argc,char* argv[]){
    if(argc==4){

        string pub = string(argv[2]);
        string mes = string(argv[3]);
        int port = atoi(argv[1]);
        if(port>1024 && port<65536){
            ifstream fpub(pub),fmes(mes);

            if(!fpub.is_open()){
                cout<<"Plik z kluczem niewczytany. Błąd!"<<endl;
            } else {
                if(!fmes.is_open()){
                    cout<<"Plik z wiadomością niewczytany. Błąd!"<<endl;
                } else {
                    string e = "",n = "",m = "";
                    getline(fpub,e);
                    getline(fpub,n);
                    fpub.close();
                    getline(fmes,m,'\0');
                    fmes.close();
                    generatemprim(port,e,n,m.substr(0,m.length()-1),mes);
                }
            }
        } else {
            cout<<"Użyj portów z zakresu 1025-65536"<<endl;
        }
    } else {
        cout << comm<<endl;
    }
}