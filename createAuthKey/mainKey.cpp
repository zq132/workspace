#include "crekey.h"

#include <iostream>

using namespace std;


int main(void)
{
    char* srcdir;
    char* dstdir;
    cin>>srcdir;
    cout<<"\ntype:";
    int a;
    cin>>a;
    cout<<"dst:";
    cin>>dstdir;
    int b = CreateKey(srcdir,dstdir,a);
    if(b == 0){
        cout<<"生成成功！"<<endl;
    }
}
