#include "Auth.h"

#include <iostream>

int result(ExitSign sign)
{
    // ExitSign a;
    switch(sign){
        case ExitSign::Listening_port: std::cout<<"Listening_port\n";break;
        case ExitSign::RUN: std::cout<<"RUN\n";break;
        case ExitSign::UNRegistered: std::cout<<"UNRegistered\n";break;
        case ExitSign::Disconnect_serv: std::cout<<"Disconnect_serv\n";break;
        case ExitSign::WithoutKEY: std::cout<<"WithoutKEY\n";break;
    }
    if(ExitSign::Listening_port == sign){
        std::cout<<"程序监听到agent需要退出\n";
    }
    if(ExitSign::RUN == sign){
        std::cout<<"程序正常运行\n";
    }
    if(ExitSign::UNRegistered == sign){
        std::cout<<"未注册\n";
    }
    if(ExitSign::Disconnect_serv == sign){
        std::cout<<"连接中断\n";
    }
    if(ExitSign::WithoutKEY == sign){
        std::cout<<"没有注册码文件\n";
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
    // Auth auth2 = Auth::GetInstance();
    std::cin>>a;
    return 0;
}
