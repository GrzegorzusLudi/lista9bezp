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
#include <signal.h>
#include <sys/time.h>

// COMPILATION:
// g++ --std=c++11 -o bserver bserver.cpp -lgmpxx -lgmp

using namespace std;

char halfbyte(char r){

    if(r<10)
        return '0'+r;
    else
        return 'a'+(r-10);
}
int digitnumber(char byte){
    int ret = 0;
    while(byte&0xFF!=0x00){
        if(byte&0x01)
            ret++;
        byte>>=1;
    }
    return ret;
}
char* generaterandom(int bytes){
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(0, 2147483647);
    char* ss;
    int dn = 0;
    string s;
    while(dn%3==0 || dn%7==0 || dn%15==0 || dn%31==0){
        char k[bytes/4];
        int ran = 0x00000000;
        for (int i = 0;i<bytes/4;i++){

            if(i%4==0)
                ran = dis(gen);
            char wyb;
            wyb = char(ran&0x0000000F);

            ran >>= 8;
            k[i] = wyb;
        }
        k[0]|=0x08;
        k[bytes/4-1]|=0x01;
        ss = new char[bytes/4];
        for (int i = 0;i<bytes/4;i++){
            ss[i]=0;
            dn+=digitnumber(k[i]);
            ss[i]=halfbyte(k[i]);
        }
    }
    return ss;
}

void prime(mpz_t cel,int keysize){

    bool probablyprime = false;
    mpz_t p,pdiv;mpz_init(p);mpz_init(pdiv);
    char* cs;
    do{
        cs = generaterandom(keysize);
        mpz_init_set_str(p,cs, 16);
        delete cs;
    } while(mpz_cmp_ui(p,0)==0);
    //free(cs);
    mpz_nextprime(cel,p);
    mpz_fdiv_q_ui(pdiv,cel,2);
    while(!mpz_probab_prime_p(pdiv,33)){
        mpz_nextprime(cel,cel);
        mpz_fdiv_q_ui(pdiv,cel,2);
    }
    //gmp_sprintf (ret,"%Zx\n", p);
}
string passwd(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 35);
    char marks[]="0123456789abcdefghijklmnopqrstuvwxyz";
    int length = 8;
    string str = "";
    for(int i =0;i<length;i++){
        str+=marks[dis(gen)];
    }
    return str;
}
long timestamp(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long ms = tp.tv_sec * 1000000 + tp.tv_usec;
    return ms;
}
char logname[] = "logfile.txt";
void setup(int keysize, char priv[], char pub[]){
        //RSA algorithm

        long ts = timestamp();
        int randombits = keysize/2;
        mpz_t p,q,n,lambdan,e,d,lambdana,gcda,pdiv;
        mpz_init(p);mpz_init(q);mpz_init(n);mpz_init(lambdan);mpz_init(e);mpz_init(d);mpz_init(lambdana);mpz_init(gcda);mpz_init(pdiv);
        gmp_randstate_t rs;
        prime(p,randombits);
        prime(q,randombits);
        mpz_mul(n,p,q);
        mpz_sub_ui(p,p,1);
        mpz_sub_ui(q,q,1);
        mpz_lcm(lambdan,p,q);
        mpz_sub_ui(lambdana,lambdan,4);
        gmp_randinit_default(rs);
        gmp_randseed_ui (rs, (unsigned int)time(0));
        do{
            mpz_urandomm(e,rs,lambdana);
            mpz_add_ui(e,e,3);
            mpz_gcd(gcda,lambdan,e);
        } while(mpz_cmp_ui(gcda,1)!=0);
        mpz_invert(d,e,lambdan);

        //write to file
        ofstream fpub,fpriv;
        char aux[randombits];
        fpub.open(pub);
        ofstream log(logname, std::ios_base::app | std::ios_base::out);
        gmp_sprintf (aux,"%Zx\n", e);
        fpub << aux;
        gmp_sprintf (aux,"%Zx\n", n);
        fpub << aux;
        fpub.close();

        fpriv.open(priv);
        gmp_sprintf (aux,"%Zx\n", d);
        fpriv << aux;
        gmp_sprintf (aux,"%Zx\n", n);
        fpriv << aux;
        fpriv.close();
        string pass = passwd();
        std::hash<std::string> hash_fn;
        cout<<"Twoje hasło: "<<pass<<endl;
        size_t hash = hash_fn(pass);
        log<<hex<<hash;
        log<<endl<<priv<<endl<<pub<<endl<<endl;
        log.close();
        ts = timestamp()-ts;
        cout<<"Generowanie klucza zajęło "<<((double)ts)/1000000<<" s."<<endl;
}
void runsocket(int port,string sd,string sn){
    int keysize = sn.length();
    while(true){
        int server_fd, new_socket, valread;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[keysize] = {0};
        char buffer2[keysize] = {0};

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                    &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( port );
        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr *)&address,
                                    sizeof(address))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        cout << "Listening on port "<<port<<endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        while(read( new_socket , buffer, keysize)){
            mpz_t message,d,n,e,f;
            //cout<<sd<<endl<<sn<<endl;
            mpz_init_set_str(message,buffer,16);
            mpz_init_set_str(d,sd.c_str(),16);
            mpz_init_set_str(n,sn.c_str(),16);
            mpz_powm(message,message,d,n);
            gmp_sprintf (buffer2,"%Zx", message);
            //gmp_printf ("%Zx\n", f);
            //printf("%s",buffer2 );
            send(new_socket , buffer2 , keysize , 0 );
        }
    }
}
//uwtf3jpo
void sign(int port,string password){
    ifstream conf(logname);
    string line;
    if(conf.is_open()){
        std::hash<std::string> hash_fn;
        size_t hash = hash_fn(password);
        std::stringstream ss;
        ss<<hex<<hash;
        string pas;
        ss>>pas;

        string priv = "";
        string pub = "";
        bool found = false;
        int i = 0;
        bool founded = false;
        while(!founded && getline(conf,line)){
            if(line=="")
                i = -1;
            else{
                switch(i){
                    case 0:
                        if(line==pas)
                            found = true;
                    break;
                    case 1:
                        if(found)
                            priv = line;
                    break;
                    case 2:
                        if(found){
                            pub = line;
                            founded = true;
                        }
                    break;
                }
            }
            i++;
        }
        if(!found){
            cout<<"Złe hasło!"<<endl;
        } else {
            ifstream prfile(priv);
            ifstream pbfile(pub);
            cout<<pub<<endl;
            if(!prfile.is_open()){
                cout<<"Plik z kluczem niewczytany. Błąd!"<<endl;
            } else {
                string d = "",n = "";
                getline(prfile,d);
                getline(prfile,n);
                prfile.close();
                runsocket(port,d,n);
            }
        }
    } else {
        cout<<"No files created"<<endl;
    }
}
char comm[] = "Sposób działania programu: \n-bserver setup [rozmiar klucza] [plik klucza pryw] [plik klucza publ], gdzie rozmiar klucza to 2048, 4096, 8192 lub 16384 \n-bserver sign [hasło] [port]";
int main(int argc,char* argv[]){
    //sigignore(SIGPIPE);
    if(argc<2){
        cout << comm << endl;
    } else {
        if(strcmp(argv[1],"setup")==0){
            if(argc==5){
                if(true || strcmp(argv[2],"2048")==0 || strcmp(argv[2],"4096")==0 || strcmp(argv[2],"8192")==0 || strcmp(argv[2],"16384")==0)
                    setup(atoi(argv[2]),argv[3],argv[4]);
                else
                    cout << comm << endl;
            } else
                cout << comm << endl;
        } else if(strcmp(argv[1],"sign")==0){
            if(argc==4){
                string password = string(argv[2]);
                int port = atoi(argv[3]);
                if(port>1024 && port<65536){
                    sign(port,password);
                } else {
                    cout<<"Użyj portów z zakresu 1025-65536"<<endl;
                }
            } else {
                cout << comm << endl;
            }
        } else {
            cout << comm << endl;
        }
    }
    return 0;
}
/* TRASH
 *
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(-5, 5);
    int dif = dis(gen);
 *
 * */