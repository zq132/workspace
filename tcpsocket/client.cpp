#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> /* netdb is necessary for struct hostent */
#include <arpa/inet.h>

#define PORT 8800 /* server port */

#define MAXDATASIZE 100
#define SERVER_IP "127.0.0.1"


int getSocket()
{
    int ret = socket(AF_INET, SOCK_STREAM, 0);
    if(ret<0)
    {
        printf("socket() error\n");
        exit(1);
    }
    return ret;
}

int getConnect(int sockfd)
{
    int ret;
    struct sockaddr_in server;

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    //server.sin_addr = *((struct in_addr *)he->h_addr);
    server.sin_addr.s_addr=inet_addr(SERVER_IP);

    ret = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (ret == -1)
    {
        printf("connect() error\n");
        exit(1);
    }
    return ret;
}


int main(int argc, char *argv[])
{
    int sockfd, num;       /* files descriptors */
    char buf[MAXDATASIZE]; /* buf will store received text */

    sockfd=getSocket();
    if(getConnect(sockfd)==-1)
    {
        printf("connect() error\n");
        exit(1);
    }

    char str[10] = {'\0'};
    while (1)
    {
        scanf("%s", str);
        if ((num = send(sockfd, str, sizeof(str), 0)) == -1)
        {
            printf("send() error\n");
            exit(1);
        }
        if ((num = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
        {
            printf("recv() error\n");
            exit(1);
        }
        buf[num - 1] = '\0';
        printf("recv server message: %s\n", buf);
    }

    close(sockfd);
    return 0;
}
