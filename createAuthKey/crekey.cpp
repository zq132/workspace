#include "crekey.h"
#include "crypto.h"

#include <iostream>

using namespace std;

int CreateKey(char* srcdir, char* dstdir, int type)
{
    FILE *fr,*fw;
    char serialno[100];
    std::string authkey;
    printf("%s\n",srcdir);
    fr = fopen(srcdir,"r");
    if(fr == NULL){
        cout<<"序列号文件不存在或路径不对！"<<endl;
        return 1;
    }
    fscanf(fr,"%s",serialno);
    fclose(fr);
    if(type==1){
        EcCrypto::CreateAuthKey1(serialno,authkey);
        authkey.insert(0,"1");
    }else if(type==2){
        EcCrypto::CreateAuthKey2(serialno,authkey);
        authkey.insert(0,"2");
    }else{
        cout<<"类别代码错误！"<<endl;
        return 2;
    }
    fw = fopen(dstdir,"w");
    fprintf(fw,"%s",authkey.c_str());
    fclose(fw);
    cout << "注册码文件已生成！" <<endl;
    return 0;
}
