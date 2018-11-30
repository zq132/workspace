// #include "crypto.h"
#include "portlisten.h"

#include <iostream>
// #include <string>

using namespace std;

int main()
{
    string str;
    cout<<"key"<<endl;
    cin>>str;
    
    int a = CheckKey(str);
    if(a == -1){
        cout<<"key wrong\n";
        return 0;
    }
    if(a == 1){
        PortListen();
        cout<<"start listen\n";
    }
    while(1){
        if (!islisten)
        {
            printf("tuichu\n");
            break;
        }
    }
    return 0;
}
