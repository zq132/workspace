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


// bool isstart = true;//监听是否启动
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
        ip++;
    }

    freeifaddrs(ifList);//释放内存
    return 0;
}

bool port ()//void *ptr
{
    int sockfd;
    int i,j,ret = 0;
    char buffer[1518] = {0};
    char *eth_head = NULL;

   if ((sockfd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0)
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

        WORD *ip;DWORD *localip;
        localip = (DWORD *)&buffer[26];
        /* IPv4 protocol flag */
        if (0x08 == eth_head[12] && 0x00 == eth_head[13])
        {
            switch (eth_head[14])
            {
                case 0x45: j=20;
                    break;
                case 0x46: j=24;
                    break;
                case 0x47: j=28;
                    break;
                case 0x48: j=32;
                    break;
                case 0x49: j=36;
                    break;
                case 0x4a: j=40;
                    break;
                case 0x4b: j=44;
                    break;
                case 0x4c: j=48;
                    break;
                case 0x4d: j=52;
                    break;
                case 0x4e: j=56;
                    break;
                case 0x4f: j=60;
                    break;
            
                default:
                    printf("报文解析出错\n");return 0;
            }
            ip = (WORD *)&eth_head[16+j];
            WORD a = ntohs(*ip);
            if(a == 8800){
                i = 0;
                while (iparr[i] > 0)
                {
                    if (*localip == iparr[i])
                    {
                        if(*localip != 16777343){
                            islisten = false;
                            break;//exit(0); //return true;
                        }
                    }
                    i++;
                }
            }
        }
   }

    return 0;
}

bool PortListen()
{
    std::thread red(port);
    red.detach();
    return false;
}

int CheckKey(const std::string &key)
{
    if (EcCrypto::AuthCheck(key))
    {
        if (key[0] == '1')
        {
            return 1;
        }
        else if (key[0] == '2')
        {
            return 0;
        }
    }
    else
    {
        printf("注册失败！注册码不正确!\n");
        return -1;
    }
}
