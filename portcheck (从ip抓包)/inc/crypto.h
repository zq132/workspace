#ifndef __EC_CRYPTO_H__
#define __EC_CRYPTO_H__

#include <iostream>

class EcCrypto
{
public:
    static bool CreateSerialNo(std::string &serialNo);
    static bool AuthCheck(const std::string &authStr);
};

#endif

