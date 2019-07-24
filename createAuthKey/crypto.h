#ifndef __EC_CRYPTO_H__
#define __EC_CRYPTO_H__

#include <iostream>

class EcCrypto
{
public:
    static bool CreateSerialNo(std::string &serialNo);
    static bool AuthCheck(const std::string &authStr);
    static bool CreateAuthKey(const std::string &serialNo, std::string &authKey);//nsagent
    static bool CreateAuthKey1(const std::string &serialNo, std::string &authKey);//listen8800
    static bool CreateAuthKey2(const std::string &serialNo, std::string &authKey);//nolisten
};

#endif

