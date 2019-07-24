#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "ecpcap.h"

int main()
{
    BYTE buf[10];
    char file[50];
    BYTE ip[4];
    int port = 8000,len,offlen,count = 0,numdsp,interval = 0;
    int numsecond;
    DWORD lastsoc = 0,soc,srcip,tempip;
    DWORD lastfracsec,fracsec;
    WORD tempport;
    pcap_file_header pfh;
    packet_header ph;
    FILE *fp;
    int i,k,m = 0;
    bool equal = false;

    printf("请输入pcap文件名，如：/home/ec/11.pcap： ");
    scanf("%s",file);
    printf("请输入上位机ip：");
    scanf("%d.%d.%d.%d",ip+0,ip+1,ip+2,ip+3);
    srcip = *(DWORD *)ip;
    srcip = ntohl(srcip);

    printf("请输入采样数：");
    scanf("%d",&interval);
    numsecond = 100 / ( interval / 48);

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
        len = (buf[0] &0xf) * 4; //ip头部长度
        offlen += len; //偏移到tcp头部
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
        fseek(fp, len+2, SEEK_CUR);
        fread(buf, 2, 1, fp);
        tempport = *(WORD *)buf;
        tempport = ntohs(tempport);
        if(tempport != port){
            fseek(fp, ph.caplen - 34 -len, SEEK_CUR);
            continue;
        }
        fseek(fp, 8, SEEK_CUR);
        fread(buf,1,1,fp);
        len = (buf[0] >>4) * 4;
        offlen += len;
        len -= 13;
        // printf("%x,%d\n",buf[0],len);
        fseek(fp, len + 4, SEEK_CUR);
        fread(buf,4,1,fp);
        soc = *(DWORD *)buf;
        soc= ntohl(soc);
        // printf("%x\n",soc);
        fread(buf, 4, 1, fp);
        fracsec = *(DWORD *)buf;
        fracsec= ntohl(fracsec);
        fracsec /= 96;
        if(lastsoc == 0){
            lastsoc = soc;
            lastfracsec = fracsec;
        }else{
            lastfracsec++;
            lastsoc += lastfracsec / numsecond;
            lastfracsec %= numsecond;
        }
        if(soc != lastsoc){
            printf("第%d帧与上一帧时刻不连续\n",count);
            lastsoc =soc;
            lastfracsec = fracsec;
        }else if(fracsec != lastfracsec){
            lastfracsec = fracsec;
            printf("第%d帧与上一帧时刻不连续\n",count);
        }
        offlen = ph.caplen - offlen - 12;
        fseek(fp, offlen, SEEK_CUR);
    }
    fclose(fp);
    printf("共%d帧，文件读取完成!\n",count);
    
    return 0;
}