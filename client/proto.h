#ifndef __PROTO_H__
#define __PROTO_H__

#include <iostream>

typedef unsigned char BYTE;//1
typedef unsigned short WORD;//2
typedef unsigned int DWORD;//4
// typedef unsigned long DDWORD;//8

struct proto
{
    DWORD sync;
    WORD len;
    DWORD serno;
    DWORD times;
    DWORD second;
    WORD frametype;
} __attribute__((packed));

// struct servermsg
// {
//     BYTE cmd;
//     BYTE isrun;
//     WORD keylen;
//     DWORD serno;
// };

#endif