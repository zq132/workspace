#include "crypto.h"

#include <iostream>

using namespace std;

int main()
{
    FILE *fd;
    std::string serial;

    cout<<"当前版本号:1.0.1"<<endl;
    EcCrypto::CreateSerialNo(serial);
    fd = fopen("serialno.txt","w");
    if(fd == NULL){
        cout<< "打开或新建文件失败！\n" <<endl;
        return 0;
    }
    fprintf(fd,"%s",serial.c_str());
    fclose(fd);
    printf("序列号文件已生成！\n");
    return 0;
}
