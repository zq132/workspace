#include "crypto.h"

#include <iostream>

using namespace std;


int main(void)
{
    FILE *fr,*fw;
    int a;
    char serialno[100];
    std::string authkey;
    cout<<"当前版本号:1.0.1"<<endl;
    fr = fopen("serialno.txt","r");
    if(fr == NULL){
        cout<<"序列号文件不存在或路径不对！"<<endl;
        return 0;
    }
    fscanf(fr,"%s",serialno);
    fclose(fr);
    cout<<"请选择生成哪种注册码"<<endl;
    cout<<"请输入类别代码（1:代表监听端口 2:代表不监听端口）：";
    cin>>a;
    if(a==1){
        EcCrypto::CreateAuthKey1(serialno,authkey);
        authkey.insert(0,"1");
    }else if(a==2){
        EcCrypto::CreateAuthKey2(serialno,authkey);
        authkey.insert(0,"2");
    }else{
        cout<<"类别代码错误！"<<endl;
        return 0;
    }
    fw = fopen("register.txt","w");
    // EcCrypto::CreateAuthKey1(serialno,authkey);
    fprintf(fw,"%s",authkey.c_str());
    fclose(fw);
    cout << "注册码文件已生成！" <<endl;
    return 0;
}
