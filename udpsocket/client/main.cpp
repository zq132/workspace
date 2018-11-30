#include "Auth.h"

#include <iostream>

void result(int a)
{
    if(a == 0){
        std::cout<<"程序需要退出\n";
    }
    if(a == 1){
        std::cout<<"程序正常运行\n";
    }
    if(a == 2){
        std::cout<<"连接中断\n";
    }
}

int main()
{
    // using namespace std;


    ClientBack ptr = result;

    Auth auth = Auth::GetInstance();

    auth.RunClient(ptr);

    // std::cout << "输入命令类型2(改时间)3(check发送注册码)" << std::endl;
    int a;
    // std::cin >> a;
    // if(a == 2){
    //     std::cout<<"输入时间:";
    //     int tmsec;
    //     std::cin>>tmsec;
    //     struct timeval tm;
    //     tm.tv_sec = tmsec;
    //     tm.tv_usec = 0;
    //     auth.SetTime(&tm);
    //     // Auth::SetTime(&tm);
    // }
    Auth auth2 = Auth::GetInstance();
    std::cin>>a;
    return 0;
}