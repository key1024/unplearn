#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>
#include "tools.h"

void str_echo(int sockfd)
{
    while(1)
    {
        char buff[1024] = {0};
        ssize_t nr = read(sockfd, buff, sizeof(buff));
        if(nr < 0)
        {
            printf("read error: %s\n", strerror(errno));
            break;
        }
        else if(nr > 0)
        {
            ssize_t nw = writen(sockfd, buff, nr);
            if(nw < 0)
            {
                printf("write error: %s\n", strerror(errno));
                break;
            }
        }
        else
        {
            break;
        }
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        printf("child %d terminated\n", pid);
    }

    return;
}

int main(int argc, char **argv)
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
       printf("bind error： %s\n", strerror(errno));
       return -1; 
    }

    if(listen(listenfd, 1024) < 0)
    {
        printf("listen error: %s\n", strerror(errno));
        return -1;
    }

    int maxfd = listenfd;
    int maxi = -1;
    int clientfd[FD_SETSIZE];
    for(int i = 0; i < FD_SETSIZE; i++)
        clientfd[i] = -1;
    
    fd_set allset, rset;
    FD_SET(listenfd, &allset);

    while(1)
    {
        rset = allset;
        int nready = select(maxfd+1, &rset, NULL, NULL, NULL);

        if(FD_ISSET(listenfd, &rset))
        {
            struct sockaddr_in cliaddr;
            socklen_t clilen = sizeof(cliaddr);
            int connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
            if(connfd == -1)
            {
                printf("accept error: %s\n", strerror(errno));
                break;
            }

            int i = 0;
            for(i = 0; i < FD_SETSIZE; i++)
            {
                if(clientfd[i] == -1)
                {
                    clientfd[i] = connfd;
                    if(maxi < i)
                        maxi = i;
                    break;
                }
            }

            // 连接的客户端过多，关闭该连接
            if(i == FD_SETSIZE)
            {
                printf("too many clients.\n");
                close(connfd);
            }
            else
            {
                if(maxfd < connfd)
                    maxfd = connfd;

                FD_SET(connfd, &allset);

                if(--nready <= 0)
                    continue;
            }
        }

        for(int i = 0; i <= maxi; i++)
        {
            if(clientfd[i] < 0)
                continue;

            if(FD_ISSET(clientfd[i], &rset))
            {
                char buff[1024] = {0};
                ssize_t nr = read(clientfd[i], buff, sizeof(buff));
                if(nr < 0) // 程序出错，退出
                {
                    printf("read error: %s\n", strerror(errno));
                    exit(-1);
                }
                else if(nr == 0) // 对方关闭链接
                {
                    printf("client closed the connection.\n");
                    FD_CLR(clientfd[i], &allset);
                    close(clientfd[i]);
                    clientfd[i] = -1;
                }
                else
                {
                    ssize_t nw = writen(clientfd[i], buff, nr);
                    if(nw < 0)
                    {
                        printf("write error: %s\n", strerror(errno));
                        exit(-1);
                    }
                }

                if(--nready <= 0)
                    break;
            }
        }
    }

    close(listenfd);

    return 0;
}