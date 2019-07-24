#ifndef _ECPCAP_H_
#define _ECPCAP_H_

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

struct pcap_file_header
{
    DWORD magic;  //读取顺序,0x1a2b3c4d,0xa1b2c3d4按原来的顺序读,0xd4c3b2a1字节交换顺序读
    WORD version_major; //0x02当前文件主版本号
    WORD version_minor; //0x04当前文件次版本号
    DWORD thiszone;  //GMT和本地时间的相差，秒来表示。
    DWORD sigfigs; //时间戳
    DWORD snaplen; //设置所抓获数据包最大长度
    DWORD linktype; //链路类型 1：以太网包
};

struct packet_header
{
    DWORD soc; //utc
    DWORD usec;//微秒
    DWORD caplen;//当前数据帧长度
    DWORD len; //网络中实际数据帧长度
};



#endif