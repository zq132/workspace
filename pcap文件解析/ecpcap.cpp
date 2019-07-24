#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "ecpcap.h"

int main()
{
    BYTE buf[10];
    char file[50];
    BYTE ip[4];
    int port = 4001,len,offlen,count = 0;
    DWORD lastsoc = 0,soc,srcip,tempip;
    BYTE lastfracsec,fracsec;
    WORD tempport;
    pcap_file_header pfh;
    packet_header ph;
    FILE *fp;

    printf("请输入pcap文件名，如：/home/ec/11.pcap： ");
    scanf("%s",file);
    printf("请输入下位机ip,如：10.10.10.10: ");
    scanf("%d.%d.%d.%d",ip+0,ip+1,ip+2,ip+3);
    srcip = *(DWORD *)ip;
    srcip = ntohl(srcip);
    // printf("%02x\n",srcip);
    // printf("请输入查询端口: ");
    // scanf("%d",&port);
    // if(port != 4001){
    //     printf("端口号错误!\n");
    //     return 0;
    // }

    fp = fopen(file,"r");
    if(fp == NULL){
        printf("Open file %s error.", file);
        return 0;
    }
    fread(&pfh, sizeof(pcap_file_header), 1, fp);
    // fseek(fp, 24,SEEK_CUR);

    if(pfh.magic != 0xa1b2c3d4){
        printf("error\n");
        fclose(fp);
        return 0;
    }
    // fread(&ph, sizeof(packet_header), 1, fp);
    while(fread(&ph, sizeof(packet_header), 1, fp) != 0){
        count ++;
        if(ph.caplen < 100){
            fseek(fp, ph.caplen, SEEK_CUR);
            continue;
        }
        fseek(fp, 14, SEEK_CUR);
        offlen = 14;
        fread(buf, 1,1,fp);
        len = (buf[0] &0xf) * 4;
        offlen += len;
        fseek(fp, 11, SEEK_CUR);
        fread(buf,4,1,fp);
        tempip = *(DWORD *)buf;
        tempip = ntohl(tempip);
        if( tempip != srcip ){
            fseek(fp, ph.caplen - 30, SEEK_CUR);
            continue;
            // printf("%x,%x",tempip, srcip);
        }
        len -= 16;
        fseek(fp, len, SEEK_CUR);
        fread(buf, 2, 1, fp);
        tempport = *(WORD *)buf;
        tempport = ntohs(tempport);
        if(tempport != port){
            fseek(fp, ph.caplen - 32 -len, SEEK_CUR);
            continue;
        }
        fseek(fp, 10, SEEK_CUR);
        fread(buf,1,1,fp);
        len = (buf[0] >>4) * 4;
        offlen += len;
        len -= 13;
        // printf("%x,%d\n",buf[0],len);
        fseek(fp, len + 2, SEEK_CUR);
        fread(buf,4,1,fp);
        soc = *(DWORD *)buf;
        soc = ntohl(soc);
        // printf("%x\n",soc);
        fseek(fp, 13, SEEK_CUR);
        fread(buf, 1, 1, fp);
        fracsec = buf[0];
        if(lastsoc == 0){
            lastsoc = soc;
            lastfracsec = fracsec;
        }else{
            lastfracsec++;
            lastsoc += lastfracsec / 100;
            lastfracsec %= 100;
        }
        if(soc != lastsoc){
            printf("第%d帧与上一帧时刻不连续\n",count);
            lastsoc =soc;
            lastfracsec = fracsec;
        }else if(fracsec != lastfracsec){
            lastfracsec = fracsec;
            printf("第%d帧与上一帧时刻不连续\n",count);
        }
        offlen = ph.caplen - offlen -20;
        fseek(fp, offlen, SEEK_CUR);
    }
    fclose(fp);
    printf("共%d帧，文件读取完成!\n",count);
    
    return 0;
}