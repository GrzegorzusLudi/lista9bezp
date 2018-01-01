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

using namespace std;

int timestamp(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    int ms = tp.tv_sec * 1000000 + tp.tv_usec;
    return ms;
}
int main(){
    string s;
    cin >> s;
    gmp_randstate_t rs;
    mpz_t n,x,y,d;
    int koniec = 0;
    mpz_init_set_str(n,s.c_str(),16);
    while(!koniec){
        if(mpz_probab_prime_p(n,32)){
            koniec = 1;
        } else {
        mpz_init_set_ui(x,2);
        mpz_init_set_ui(y,2);
        mpz_init_set_ui(d,1);
        gmp_randinit_default(rs);
        while(mpz_cmp_ui(d,1)==0){
            gmp_randseed_ui (rs, timestamp());
            mpz_urandomm(x,rs,n);
            gmp_randseed_ui (rs, timestamp());
            mpz_urandomm(y,rs,n);
            mpz_sub(d, x, y);
            mpz_abs(d, d);
            mpz_gcd(d,d,n);
            if(mpz_cmp(d,n)==0){
                koniec = 1;
            }
        }
        if(!koniec){
            gmp_printf ("%Zx\n", d);
            gmp_printf (">%Zx\n", n);
            mpz_fdiv_q(n,n,d);
        }
        }
    }
    return 0;
}