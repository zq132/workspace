// #include "crypto.h"
#include "portlisten.h"

#include <iostream>
#include <unistd.h>
// #include <string>

using namespace std;

int main()
{
    extern bool islisten;
    // if(CheckKey()){
    //     PortListen();
    // }
    port();
    while(1){
        if (!islisten)
        {
            printf("tuichu\n");
            break;
        }
    }
    return 0;
}
