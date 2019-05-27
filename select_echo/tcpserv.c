#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
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

    signal(SIGCHLD, sig_chld);

    while(1)
    {
        struct sockaddr_in cliaddr;
        socklen_t clilen = sizeof(cliaddr);
        int connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
        if(connfd == -1)
        {
            printf("accept error: %s\n", strerror(errno));
            break;
        }

        pid_t childpid = fork();
        if(childpid == -1) // 创建子进程出错
        {
            printf("fork error: %s\n", strerror(errno));
            break;
        }
        else if(childpid == 0) // 子进程
        {
            close(listenfd);
            // 回显客户端的输入
            str_echo(connfd);

            printf("子进程退出\n");

            exit(0);
        }

        close(connfd);
    }

    close(listenfd);

    return 0;
}