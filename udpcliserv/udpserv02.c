#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

static int count;

void recvfrom_int(int signo)
{
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

int main(int argc, char **argv)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(9999);

    int ret = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(ret < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    signal(SIGINT, recvfrom_int);

    while(1)
    {
        char msg[8] = {0};
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, msg, 8, 0, (struct sockaddr*)&cliaddr, &len);
        if(n < 0)
        {
            printf("recvfrom error: %s\n", strerror(errno));
            break;
        }

        printf("recv msg: %s\n", msg);

        count++;
    }

    close(sockfd);

    return 0;
}