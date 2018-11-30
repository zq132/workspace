#ifndef __PROTO_H__
#define __PROTO_H__

#include <iostream>

typedef unsigned char BYTE;//1
typedef unsigned short WORD;//2
typedef unsigned int DWORD;//4
// typedef unsigned long DDWORD;//8

    struct proto {
        DWORD sync;
        WORD len;
        DWORD serno;
        DWORD times;
        DWORD second;
        WORD frametype;
    }__attribute__((packed));

    // struct eth_hdr {
    //     BYTE dstmac[6];//目的mac
    //     BYTE srcmac[6];//源mac
    //     WORD eth_type;//以太网类型
    // }__attribute__((packed));

    // struct ip_hdr {
    //     #if __BYTE_ORDER == __LITTLE_ENDIAN
    //     BYTE ihl:4; //首部长度
    //     BYTE version:4; //版本
    //     #else
    //     BYTE version:4;
    //     BYTE ihl:4;
    //     #endif
    //     BYTE tos;//服务类型
    //     WORD tot_len;//总长度
    //     WORD id;//标志
    //     WORD frag_off;//分片偏移
    //     BYTE ttl;//生存时间
    //     BYTE protocol;//协议
    //     WORD chk_sum;//检验和

    // };

    // struct tcp_hdr {};

    // struct udp_hdr {};

#endif