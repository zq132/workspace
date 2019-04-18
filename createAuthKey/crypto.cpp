#include "crypto.h"

#include <arpa/inet.h>
#include <openssl/md5.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <linux/hdreg.h>

bool GetCpuID(std::string &cpuid)
{
    cpuid.clear();  
    
    unsigned int s1 = 0;  
    unsigned int s2 = 0;  
    asm volatile  
    (  
        "movl $0x01, %%eax; \n\t"  
        "xorl %%edx, %%edx; \n\t"  
        "cpuid; \n\t"  
        "movl %%edx, %0; \n\t"  
        "movl %%eax, %1; \n\t"  
        : "=m"(s1), "=m"(s2)  
    );  
  
    if (0 == s1 && 0 == s2) {  
        return(false);  
    } 
  
    char cpu[32] = { 0 };  
    snprintf(cpu, sizeof(cpu), "%08X%08X", htonl(s2), htonl(s1));  
    std::string(cpu).swap(cpuid);  
    return(true);  
}

bool GetDiskSerial(std::string &diskid)
{
    diskid.clear();

    struct hd_driveid id;
    int fd = open("/dev/sda", O_RDONLY | O_NONBLOCK);
    //失败返回
    if (fd < 0)
    {
        perror("/dev/sda");
        exit(0);
    }
    //调用ioctl()
    if (!ioctl(fd, HDIO_GET_IDENTITY, &id))
    {
        char str[32] = {'\0'};
        snprintf(str, sizeof(str), "%s", id.serial_no);
        std::string(str).swap(diskid);
        return true;
    }
    return false;
}

bool EcCrypto::CreateSerialNo(std::string &serial)
{
    serial.clear();

    std::string cpuid;
    GetCpuID(cpuid);
    std::string diskid;
    GetDiskSerial(diskid);

    cpuid += diskid;
    if (cpuid.empty())
     {
         return false;
     }

    unsigned char md[16];
    MD5((unsigned char*)cpuid.c_str(),cpuid.size(),md);
    char temp[3] = {'\0'};
    for( int i = 0; i < 16; ++i ){
	sprintf(temp, "%2.2X", md[i]);
	serial += temp;
    }
    return true;
}

bool EcCrypto::CreateAuthKey1(const std::string &serialNo, std::string &authKey)  //listen8800
{
    authKey.clear();

    char str2[24]={'\0'};
    std::string v(serialNo + "ecauth-1+W87");
//str2数组作用是抽取23个序列号中的字符，并将其附加在当前序列号后，以下为随机选取的抽字符的过程
    str2[0]=v[35];
    str2[1]=v[23];
    str2[2]=v[14];
    str2[3]=v[7];
    str2[4]=v[2];
    str2[5]=v[25];
    str2[6]=v[6];
    str2[7]=v[10];
    str2[8]=v[30];
    str2[9]=v[5];
    str2[10]=v[24];
    str2[11]=v[9];
    str2[12]=v[17];
    str2[13]=v[27];
    str2[14]=v[12];
    str2[15]=v[18];
    str2[16]=v[39];
    str2[17]=v[19];
    str2[18]=v[16];
    str2[19]=v[0];
    str2[20]=v[32];
    str2[21]=v[13];
    str2[22]=v[36];

    //std::string v;
    //v=serialNo+str2;
    v+=str2;
    unsigned char md[16];
    char tmp[3] = {'\0'};
    MD5((unsigned char*)v.c_str(), v.size(), md);
    for( int i = 0; i < 16; ++i ){
	sprintf(tmp, "%2.2X", md[i]);
	authKey += tmp;
    }
    return true;
}

bool EcCrypto::CreateAuthKey2(const std::string &serialNo, std::string &authKey) //nolisten
{
    authKey.clear();

    char str2[24]={'\0'};
    std::string v(serialNo + "ecauth-1+W87");
//str2数组作用是抽取23个序列号中的字符，并将其附加在当前序列号后，以下为随机选取的抽字符的过程
    str2[0]=v[16];
    str2[1]=v[34];
    str2[2]=v[9];
    str2[3]=v[30];
    str2[4]=v[23];
    str2[5]=v[35];
    str2[6]=v[29];
    str2[7]=v[7];
    str2[8]=v[5];
    str2[9]=v[17];
    str2[10]=v[32];
    str2[11]=v[28];
    str2[12]=v[43];
    str2[13]=v[19];
    str2[14]=v[27];
    str2[15]=v[0];
    str2[16]=v[1];
    str2[17]=v[25];
    str2[18]=v[4];
    str2[19]=v[11];
    str2[20]=v[24];
    str2[21]=v[20];
    str2[22]=v[41];

    //std::string v;
    //v=serialNo+str2;
    v+=str2;
    unsigned char md[16];
    char tmp[3] = {'\0'};
    MD5((unsigned char*)v.c_str(), v.size(), md);
    for( int i = 0; i < 16; ++i ){
	sprintf(tmp, "%2.2X", md[i]);
	authKey += tmp;
    }
    return true;
}

bool EcCrypto::CreateAuthKey(const std::string &serialNo, std::string &authKey) //nsagent
{
	authKey.clear();

	char str2[24] = {'\0'};
	std::string v(serialNo + "ecauth-1+W87");
	// str2数组作用是抽取23个序列号中的字符，并将其附加在当前序列号后，以下为随机选取的抽字符的过程
	str2[0] = v[24];
	str2[1] = v[30];
	str2[2] = v[18];
	str2[3] = v[27];
	str2[4] = v[20];
	str2[5] = v[39];
	str2[6] = v[22];
	str2[7] = v[17];
	str2[8] = v[15];
	str2[9] = v[7];
	str2[10] = v[3];
	str2[11] = v[29];
	str2[12] = v[34];
	str2[13] = v[2];
	str2[14] = v[11];
	str2[15] = v[10];
	str2[16] = v[21];
	str2[17] = v[12];
	str2[18] = v[14];
	str2[19] = v[31];
	str2[20] = v[34];
	str2[21] = v[10];
	str2[22] = v[40];

	v += str2;
	unsigned char md[16];
	char tmp[3] = {'\0'};
	MD5((unsigned char *)v.c_str(), v.size(), md);
	for (int i = 0; i < 16; ++i)
	{
		sprintf(tmp, "%2.2X", md[i]);
		authKey += tmp;
	}
	return true;
}

bool EcCrypto::AuthCheck(const std::string &auth)
{
    std::string serial;
    CreateSerialNo(serial);

    // char s = auth[0];
    std::string str = auth;
    str.erase(0,1);
    std::string key;

    if(auth[0] == '1'){
        CreateAuthKey1(serial,key);
    }else if(auth[0] == '2'){
        CreateAuthKey2(serial,key);
    }
    
    // CreateAuthKey(serial, key);
    if( key == str ){
	return true;
    }
    return false;
}
