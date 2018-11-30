#include "portlisten.h"
#include "proto.h"
#include "utility.h"
#include "server.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <thread>

#define PORT 9876
#define CLIPORT 12345
#define MAXDATASIZE 2048

DWORD lastserno = 0;//last
DWORD ackserno = 0;//确认序号
int sockfd = 0;
bool exitfd = false;//退出标志，为真时退出程序

std::string Getstr(char *buf, int start, int end)
{
    std::string str = "";
    for(int i = start; i<end+1; i++)
    {
        str += buf[i];
    }
    return str;
}

void ClearArr(char *buf,int length)
{
    for(int i=0; i<length; i++)
    {
        buf[i]=0;
    }
}

void MyStrcat(char *str1, char *str2, int pos, int len)  //拼接函数
{
    for (int i = 0;i < len;i++)
    {
        str1[pos + i] = str2[i];
    }
}

void SendData(int sockfd, int type)  //客户端, DWORD serno, char *key, int keylen
{
    int length;
    
    switch (type)
    {
        case 1:
            length = 22 + 2;
            break;
        case 4:
            length = 23;
            break;
        case 5:
            length = 22 + 5;
            break;
        case -1:
            length = 22 + 2;
            break;
    }

    char buf[length] = {0};
    char *ptr;
    WORD *wordptr;
    DWORD *dwordptr;
    char str[8]={0};
    int tmpnum;
    std::string tmpstr;
    struct timeval tv;
    struct proto *message;
    message = (struct proto *)buf;
    message->sync = 0x594B4B59;
    message->len = length;
    message->serno = lastserno + 1;
    message->frametype = 0x0101;

    gettimeofday(&tv,NULL);
    message->times = tv.tv_sec;
    message->second = 0x0;

    if(type == -1){//程序退出命令包(注册码检测失败,监听到端口退出)
        buf[20] = 0x01;
        buf[21] = 0x00;
        // if(islisten == false){ //isstart判断监听程序是否启动
        //     buf[21] = 0x0;
        // }
    }
    if(type == 1){
        buf[20] = 0x01;
        buf[21] = 0x01;
    }
    if(type == 4){
        buf[20] = 0x04;//心跳包
    }
    if(type == 5){
        buf[20] = 0x05;
        dwordptr = (DWORD *)&buf[21];
        *dwordptr = lastserno;//上一个序号
    }

    WORD crc = ecutility::crc16((unsigned char *)buf, length -2);

    wordptr = (WORD*)&buf[length-2];
    *wordptr = crc;
    // if (type != 4)
    // {
    //     printf("%04X\n", crc);
    //     for (int i = 0; i < length; i++)
    //     {
    //         printf("%02hhX ", buf[i]);
    //     }
    //     printf("\n");
    // }

    if(type != 5 && type != 4){
    ackserno = message->serno;//确认包不用在确认
    }
    lastserno = message->serno;

    send(sockfd, buf, length, 0);
}

void DealData(int sockfd, char *buf,int length)
{
    // BYTE *byteptr;
    WORD *wordptr;
    DWORD *dwordptr;
    struct proto *msg = (struct proto *)buf;

    // printf("recv：");
    // // break;
    // for (int i = 0; i < length; i++)
    // {
    //     printf("%02hhX ", buf[i]);
    // }
    // printf("\n");
    //检验crc
    wordptr = (WORD *)&buf[length-2];

    int crc = ecutility::crc16((unsigned char *)buf, length-2);
    if (crc != *wordptr)
    {
        //crc错误,不处理数据，直接返回
        return;
    }
    lastserno = msg->serno;
    if (buf[20] != 5)//确认包
    {
        // lastserno = msg->serno;
        SendData(sockfd, 5);
    }
    if(buf[20] == 2){
        dwordptr = (DWORD *)&buf[21];
        struct timeval tmpv;
        tmpv.tv_sec = *dwordptr;
        tmpv.tv_usec = *(dwordptr +1);
        settimeofday(&tmpv,NULL);
    }
    if(buf[20] == 3){
        wordptr = (WORD *)&buf[21];
        int len = *wordptr;
        char *p = &buf[23];
        buf[length-2]='\0';
        int isrun = CheckKey(p);
        if(isrun == -1){
            SendData(sockfd, -1);
            return;
        }
        if(isrun == 1){
            //注册成功，并且需要监听
            PortListen();
        }
        SendData(sockfd, 1);
    }
    if(buf[20] == 5){
        dwordptr = (DWORD *)&buf[21];
        if(*dwordptr != ackserno){
            SendData(sockfd, 1);
        }
    }
    
}

void Heart()
{
    struct timeval waitd;
    while(1){
        if(exitfd){
            break;
        }
        waitd.tv_sec = 2;
        waitd.tv_usec = 0;
        select(0,0,0,0,&waitd);
        SendData(sockfd, 4);
    }
}

void RunServer()
{
    extern bool islisten;
    // int sockfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    int errmsg,bytes=0;
    fd_set read_flags,write_flags;
    struct timeval waitd;
    char buf[MAXDATASIZE] = {0};
    exitfd = false;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Creating socket failed.\n");
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
    if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Bind() error.\n");
        exit(1);
    }
    if(connect(sockfd,(struct sockaddr *)&client,sizeof(client))== -1){
        printf("connect() error.\n");
        exit(1);
    }
    // ioctl(sockfd, FIONBIO, 1); //非阻塞模式，0是阻塞

    std::thread cli(Heart);
    cli.detach();

    while(1)
    {
        if(exitfd){
            cli.join();
            break;
        }
        waitd.tv_sec = 2;
        waitd.tv_usec = 0;
        if(islisten == false){
            char str[] = "监听成功\n";
            SendData(sockfd,-1);
            islisten = true;
            continue;
        }
        FD_ZERO(&read_flags);
        FD_SET(sockfd,&read_flags);
        errmsg = select(sockfd+1,&read_flags,0,0,&waitd);
        if(errmsg == 0) continue;
        if(FD_ISSET(sockfd,&read_flags)){
            FD_CLR(sockfd,&read_flags);
            memset(&buf,0,sizeof(buf));
            bytes = recv(sockfd, buf, sizeof(buf)-1, 0);
            if(bytes>0){
                DealData(sockfd,buf,bytes);
            }
        }
    }
    close(sockfd);
}

void CloseServer()
{
    exitfd = true;
}