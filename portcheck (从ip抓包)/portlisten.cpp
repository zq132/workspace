#include "crypto.h"
#include "portlisten.h"
#include "proto.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <thread>


bool isport=true;//是否监听
bool islisten = true;//监听线程监听成功退出
DWORD iparr[10] = {0};//存放本机ip地址
/**
 * Set misc mode for interface
 * \param if_name interface name we will set
 * \param sockfd the socket id we will set
 * */
int set_promisc (int sockfd)
{
    struct ifreq ifr;    //获得单个接口的信息

    struct sockaddr_in *sin = NULL;
    struct ifaddrs *ifa = NULL, *ifList;
    DWORD *ip = iparr;

    if (getifaddrs(&ifList) < 0)  //获取本机接口（网卡）列表
    {
        return -1;
    }

    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)
    {
        if(ifa->ifa_addr->sa_family == AF_INET)
        {
            strcpy(ifr.ifr_name, ifa->ifa_name);
            if (0 != ioctl(sockfd, SIOCGIFFLAGS, &ifr)) //SIOCGIFFLAGS获取接口标志（获取if_name的接口标志）
            {
                printf("Get interface flag failed\n");
                return -1;
            }

            /* add the misc mode */
            ifr.ifr_flags |= IFF_PROMISC;

            if (0 != ioctl(sockfd, SIOCSIFFLAGS, &ifr))
            {
                printf("Set interface flag failed\n");
                return -1;
            }
        }
        
        sin = (sockaddr_in *)ifa->ifa_addr;
        *ip = sin->sin_addr.s_addr;
        printf("%d\n",*ip);
        ip++;
    }

    freeifaddrs(ifList);//释放内存
    return 0;
}

bool port ()
{
    int sockfd;
    int i,j,ret = 0;
    char buffer[1518] = {0};
    char *eth_head = NULL;
    BYTE bytenum,*ip;
    // BYTE *ip;
    WORD *dstport;
    DWORD *srcip;
    

   if ((sockfd = socket (AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)//(sockfd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL)))
   {
       printf ("create socket failed\n");
       exit(0);
   }

   if (0 != set_promisc (sockfd))        //wlp4s0f0  本机无线网口  enp5s0  本机有线网口
   {
       printf ("Failed to set interface promisc mode\n");
       exit(0);
   }

   while (1)
   {
        memset (buffer, 0x0, sizeof (buffer));
	    ret = recv(sockfd, buffer, sizeof (buffer), 0);

        eth_head = buffer;

        if(ret > 0){
            printf("\n%d",ret);
            for(int i = 0; i<100; i++)
            {
                if(i%4 == 0)printf("\n");
                printf("%02hhx ",buffer[i]);
            }
        }

        if(ret>0){
            printf("\n抓到数据长度：%d",ret);
            srcip = (DWORD *)&buffer[12];//获取源ip
            //首部长度
            bytenum = 0x0f;
            bytenum = bytenum & eth_head[0];
            j = bytenum * 4;
            printf("\n%x,%d",eth_head[0],bytenum);
            //ip协议
            bytenum = eth_head[0] >> 4;
            printf(",%d\n",bytenum);
            if(bytenum != 4){
                printf("不是ipv4协议\n");
                continue;
            }
            if(eth_head[6] & 0x40){
                printf("未分片\n");
            }else if(eth_head[6] & 0x20){
                printf("分片\n");
            }else{
                printf("last one\n");
            }
            dstport = (WORD *)&eth_head[6];
            WORD ptr = *dstport;
            printf("片偏移%d\n", ntohs(ptr) & 0x1fff);

            DWORD *seq = (DWORD *)&eth_head[4+j];//seq
            printf("seq:%08x  ", ntohl(*seq));
            printf("ack:%08x\n", ntohl(*(seq+1)));
            BYTE *byteptr = (BYTE *)&eth_head[12+j];
            printf("tcp首部:%02hhx,%d\n",*byteptr,*byteptr >> 4);

            dstport = (WORD *)&eth_head[2+j];
            printf("source port:%d\n", ntohs(*(dstport - 1)));
            printf("des port:%d\n", ntohs(*dstport));
            WORD a = ntohs(*dstport);
            if(a == 8800){
                i = 0;
                while (iparr[i] > 0)
                {
                    if (*srcip == iparr[i])
                    {
                        ip = (BYTE *)&eth_head[26];
                        
                        if (*srcip != 16777343) //源ip不是127.0.0.1
                        {
                            printf("source ip:");
                            for (int i = 0; i < 4; i++)
                            {
                                printf("%d.", *(ip + i));
                            }
                            printf("des ip:");
                            for (int i = 0; i < 4; i++)
                            {
                                printf("%d.", *(ip + 4 + i));
                            }
                            printf("\nsource port:%d\n", ntohs(*(dstport - 1)));
                            printf("des port:%d\n", ntohs(*dstport));
                            for (i = 0; i < 80; i++)
                            {
                                if (i % 4 == 0)
                                    printf("\n");
                                printf("%02hhx ", eth_head[14 + i]);
                            }
                            islisten = false;
                            exit(0); //break; //return true;
                        }
                    }
                    i++;
                }
            }
        }
        

        // // printf("\n协议:%04x\n",ntohs(eth_hd->h_proto));
        // WORD *ip;DWORD *srcip;
        // BYTE *ip;
        // // ip = (BYTE *)&buffer[26];
        // // printf("源ip:");
        // // for(int i = 0; i<4;i++){
        // //     printf("%hhu.",*(ip+i));
        // // }
        // srcip = (DWORD *)&buffer[26];
        // // printf("%d\n目的ip:",*srcip);
        // // for(int i = 0; i<4;i++){
        // //     printf("%hhu.",*(ip+4+i));
        // // }
        // // printf("%d\n",*(srcip+1));
        // /* IPv4 protocol flag */
        // if (0x08 == eth_head[12] && 0x00 == eth_head[13])
        // {

        //     switch (eth_head[14])
        //     {
        //     case 0x45:
        //         j = 20;
        //         break;
        //     case 0x46:
        //         j = 24;
        //         break;
        //     case 0x47:
        //         j = 28;
        //         break;
        //     case 0x48:
        //         j = 32;
        //         break;
        //     case 0x49:
        //         j = 36;
        //         break;
        //     case 0x4a:
        //         j = 40;
        //         break;
        //     case 0x4b:
        //         j = 44;
        //         break;
        //     case 0x4c:
        //         j = 48;
        //         break;
        //     case 0x4d:
        //         j = 52;
        //         break;
        //     case 0x4e:
        //         j = 56;
        //         break;
        //     case 0x4f:
        //         j = 60;
        //         break;

        //     default:
        //         printf("报文解析出错\n");
        //         return 0;
        //     }

        //     ip = (WORD *)&eth_head[16+j];
        //     WORD a = ntohs(*ip);
        //     // printf("%d\n",a);
        //     if(a == 8800){
        //         i = 0;
        //         while (iparr[i] > 0)
        //         {
        //             // printf("101   %d\n",iparr[i]);
        //             if (*srcip == iparr[i])
        //             {
        //                 // printf("IPv4 source ip:%hhu.%hhu.%hhu.%hhu,destination ip:%hhu.%hhu.%hhu."
        //                 //        "%hhu;\n",
        //                 //        eth_head[26], eth_head[27], eth_head[28],
        //                 //        eth_head[29], eth_head[30], eth_head[31],
        //                 //        eth_head[32], eth_head[33]);
        //                 // printf("source port:%02hhx%02hhx\n", eth_head[34], eth_head[35]);
        //                 // printf("des port:%02hhx%02hhx\n", eth_head[36], eth_head[37]);
                        // ip = (BYTE *)&eth_head[26];
                        

        //                 //if(eth_head[])
        //                 if (*srcip != 16777343)
        //                 {
        //                     printf("source ip:");
        //                     for (int i = 0; i < 4; i++)
        //                     {
        //                         printf("%d.", *(ip + i));
        //                     }
        //                     printf("des ip:");
        //                     for (int i = 0; i < 4; i++)
        //                     {
        //                         printf("%d.", *(ip + 4 + i));
        //                     }
        //                     printf("\nsource port:%d\n", ntohs(*(ip - 1)));
        //                     printf("des port:%d\n", ntohs(*ip));
        //                     for (i = 0; i < 80; i++)
        //                     {
        //                         if (i % 4 == 0)
        //                             printf("\n");
        //                         printf("%02hhx ", eth_head[14 + i]);
        //                     }
        //                     islisten = false;
        //                     exit(0); //break; //return true;
        //                 }
        //             }
        //             i++;
        //         }
        //     }
        // }

   }

    return 0;
}

// bool PortListen()//std::string authkey
// {
//     if (isport)
//     {
//         std::thread red(port);
//         red.detach();
//     }
//     return false;
// }

// bool CheckKey()
// {
//     FILE *fd;
//     char str[100];
//     fd=fopen("register.txt","r");
    
//     if(fd==NULL){
//         printf("请先告知管理员注册！\n");
//         return false;
//     }
//     fscanf(fd,"%s",str);
//     fclose(fd);

//     if (str[0] == '1')
//     {
//         isport = true;
//     }
//     else if(str[0] == '2')
//     {
//         isport = false;
//     }
//     if (EcCrypto::AuthCheck(str))
//     {
//         //port();
//         return true;
//     }
//     else
//     {
//         printf("注册失败！注册码不正确!\n");
//         return false;
//     }
// }
