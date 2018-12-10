#include "utility.h"
#include "proto.h"
#include "Auth.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <thread>
#include <glog/logging.h>

#define PORT 9876 //服务端端口号
#define CLIPORT 12345 //本程序端口号
#define MAXDATASIZE 2048 //缓冲区大小

DWORD lastserno = 0;//上次发送或接受的包的序号
DWORD ackserno = 0; //确认序号
bool iskill = true;//是否关闭线程
int Check = 2;//程序是否运行（注册码错误或监听到端口）
int count = 0;//判断是否中断，记录心跳包丢失情况
int sockfd = 0;//udp标识符
int lasttype = 0;//上次发送的包的种类
char *Authkey;//保存注册码
ClientBack cb = NULL;//回调函数

/*
 * 0：程序需要退出
 * 1：程序正常运行
 * 2: 连接中断
*/
int cur_status = -1;            



void ClearArr(char *buf, int length)
{
    for (int i = 0; i < length; i++)
    {
        buf[i] = 0;
    }
}

std::string Getstr(char *buf, int start, int end)
{
    std::string str = "";
    for (int i = start; i < end + 1; i++)
    {
        str += buf[i];
    }
    return str;
}

void MyStrcat(char *str1, char *str2, int pos, int len) //拼接函数
{
    for (int i = 0; i < len; i++)
    {
        str1[pos + i] = str2[i];
    }
}

void SendData(int sockfd, int type, char *key, int keylen, struct timeval *tmsec) //客户端, DWORD serno
{
    int length;

    switch (type)
    {
    case 2:
        length = 22 + 9;
        break;
    case 3:
        length = 22 + 3 + keylen;
        break;
    case 5:
        length = 22 + 5;
        break;
    }

    char buf[length] = {0};
    char *ptr;
    WORD *wordptr;
    DWORD *dwordptr;
    char str[8] = {0};
    int tmpnum;
    std::string tmpstr;
    struct timeval tv;
    struct proto *message;
    message = (struct proto *)buf;
    message->sync = 0x594B4B59;
    message->len = length;
    message->serno = lastserno + 1;
    message->frametype = 0x0101;

    gettimeofday(&tv, NULL);
    message->times = tv.tv_sec;
    message->second = 0x0;

    if (type == 2)
    {
        buf[20] = 0x02;
        dwordptr = (DWORD *)&buf[21];
        *dwordptr = tmsec->tv_sec;
        *(dwordptr + 1) = tmsec->tv_usec;
        // lasttype = 2;
        // ackserno = message->serno;
    }
    if (type == 3)
    {
        buf[20] = 0x03;
        wordptr = (WORD *)&buf[21];
        *wordptr = keylen;
        // lasttype = 3;
        // ackserno = message->serno;
        MyStrcat(buf, key, 23, keylen);
    }
    if (type == 5)
    {
        buf[20] = 0x05;
        dwordptr = (DWORD *)&buf[21];
        *dwordptr = lastserno;
    }

    WORD crc = ecutility::crc16((unsigned char *)buf, length - 2);

    wordptr = (WORD *)&buf[length - 2];
    *wordptr = crc;

    // printf("%04X\n", crc);
    // for (int i = 0; i < length; i++)
    // {
    //     printf("%02hhX ", buf[i]);
    // }
    // printf("\n");
    if(type != 5){
        lasttype = type;
        ackserno = message->serno;
    }
    lastserno = message->serno;

    send(sockfd, buf, length, 0);
}

void DealData(int sockfd, char *buf, int length)
{

    // if (buf[20] != 4)
    // {
    //     printf("recv：");
    //     // break;
    //     for (int i = 0; i < length; i++)
    //     {
    //         printf("%02hhX ", buf[i]);
    //     }
    //     printf("\n");
    // }

    WORD *wordptr;
    DWORD *dwordptr;
    struct proto *msg;

    msg = (struct proto *)buf;

    wordptr = (WORD *)&buf[length - 2];

    int crc = ecutility::crc16((unsigned char *)buf, length - 2);
    if (crc != *wordptr)
    {
        //crc错误,不处理数据，直接返回
        return;
    }
    lastserno = msg->serno;//收到的包序号
    if (buf[20] == 1)
    {
        if (buf[21] == 0)
        {
            Check = 0;
        }
        if (buf[21] == 1)
        {
            Check = 1;
        }
        if(cb != NULL){
            cur_status = Check;
            cb(Check);
        }
        SendData(sockfd, 5, NULL, 0, NULL);
    }
    if (buf[20] == 4)
    {
        count = 0;
    }
    if (buf[20] == 5)
    {
        dwordptr = (DWORD *)&buf[21];
        if (*dwordptr != ackserno)
        {
            if(lasttype == 3){
                SendData(sockfd, 3, Authkey, strlen(Authkey), NULL);
            }
        }
    }
}

void AuthCheck()
{
    FILE *fd;
    char str[100];
    std::string path = ecutility::GetAppDir();
    path += "/register.txt";

    fd = fopen(path.c_str(),"r");
    
    if( fd == NULL && cb != NULL ){
        cb(3);
        cur_status = 0;
        return;
    }
    fscanf(fd,"%s",str);
    fclose(fd);

    Authkey = str;
    SendData(sockfd, 3, str, strlen(str), NULL);
}

void Client()
{
    
    int num, errmsg, bytes = 0;
    char recvbuf[MAXDATASIZE];
    struct sockaddr_in server, client;
    struct timeval waitd;
    fd_set read_flags;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket() error\n");
        exit(1);
    }
    bzero(&server, sizeof(server));
    bzero(&client, sizeof(client));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_family = AF_INET;
    client.sin_port = htons(CLIPORT);
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        perror("Bind() error.\n");
        exit(1);
    }
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("connect() error.\n");
        exit(1);
    }

    AuthCheck();//发注册码

    while (1)
    {
        if (count >= 15)
        {
            cur_status = 0;
            //cb(2);
            cb(cur_status);
        }
        if(!iskill){
            break;
        }
        // printf("1\n");
        waitd.tv_sec = 2;
        waitd.tv_usec = 0;
        FD_ZERO(&read_flags);
        FD_SET(sockfd, &read_flags);
        errmsg = select(sockfd + 1, &read_flags, 0, 0, &waitd);
        if( errmsg == 0 ){
            count++;
            cb(cur_status);
            continue;
        }
        if (FD_ISSET(sockfd, &read_flags))
        {
            // printf("11\n");
            FD_CLR(sockfd,&read_flags);
            memset(&recvbuf,0,sizeof(recvbuf));
            bytes = recv(sockfd, recvbuf, sizeof(recvbuf) - 1, 0);
            if (bytes > 0)
            {
                DealData(sockfd, recvbuf, bytes);
            }
        }
    }
    close(sockfd);
}

void Auth::SetTime(struct timeval *tmsec)
{
    SendData(sockfd, 2, NULL, 0, tmsec);
}

void Auth::RunClient(ClientBack ptr)
{
    iskill = true;
    cb = ptr;
    std::thread cli(Client);
    cli.detach();
}

void Auth::Destroy()
{
    iskill = false;
    delete this;
}

Auth& Auth::GetInstance()
{
    static Auth ins;
    return ins;
}