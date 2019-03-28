#include "crypto.h"

#include <arpa/inet.h>
#include <openssl/md5.h>

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

bool EcCrypto::CreateSerialNo(std::string &serial)
{
    serial.clear();

    std::string cpuid;
    GetCpuID(cpuid);
    if( cpuid.empty() ){
	return false;
    }

    std::string str;
    unsigned char md[16];
    MD5((unsigned char*)cpuid.c_str(),cpuid.size(),md);
    char temp[3] = {'\0'};
    for( int i = 0; i < 16; ++i ){
	sprintf(temp, "%2.2x", md[i]);
	serial += temp;
    }
    return true;
}

bool CreateAuthKey(const std::string &serialNo, std::string &authKey)
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
	sprintf(tmp, "%2.2x", md[i]);
	authKey += tmp;
    }
    return true;
}

bool EcCrypto::AuthCheck(const std::string &auth)
{
    std::string serial;
    CreateSerialNo(serial);
    std::string key;
    CreateAuthKey(serial, key);
    if( key == auth ){
	return true;
    }
    return false;
}
