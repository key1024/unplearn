#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include "tools.h"

void str_cli(int sockfd)
{
    char sendline[1024] = {0};
    char readline[1024] = {0};

    int maxfdp1 = 0;
    int stdineof = 0;
    fd_set rset;
    FD_ZERO(&rset);

    while(1)
    {
        if(stdineof == 0)
            FD_SET(fileno(stdin), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = max(fileno(stdin), sockfd) + 1;
        if(select(maxfdp1, &rset, NULL, NULL, NULL) < 0)
        {
            printf("select error: %s\n", strerror(errno));
            return;
        }

        if(FD_ISSET(sockfd, &rset))
        {
            int nr = read(sockfd, readline, sizeof(readline));
            if(nr < 0)
            {
                printf("read error: %s\n", strerror(errno));
                return;
            }
            else if(nr == 0)
            {
                printf("对方关闭连接，通话结束\n");
                return;
            }

            if(write(fileno(stdout), readline, nr) != nr)
            {
                printf("输出到屏幕错误\n");
                return;
            }
            memset(readline, 0, sizeof(readline));
        }

        if(FD_ISSET(fileno(stdin), &rset))
        {
            int nr = read(fileno(stdin), sendline, sizeof(sendline));
            if(nr < 0)
            {
                printf("read error: %s\n", strerror(errno));
                return;
            }
            else if(nr == 0)
            {
                stdineof = 1;
                shutdown(sockfd, SHUT_WR); // send FIN
                FD_CLR(fileno(stdin), &rset);
                continue;
            }

            if(writen(sockfd, sendline, strlen(sendline)) < 0)
            {
                printf("write error: %s\n", strerror(errno));
                return;
            }
            memset(sendline, 0, sizeof(sendline));
        }
    }
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: tcpcli <IPAddress>\n");
        return -1;
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) != 1)
    {
        printf("inet_pton error, ip string: %s, error message: %s\n", argv[1], strerror(errno));
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error: %s\n", strerror(errno));
        return -1;
    }

    str_cli(sockfd);

    return 0;
}