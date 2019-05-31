#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: udpcli <IPaddress>\n");
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) != 1)
    {
        printf("inet_pton error: %s\n", argv[1]);
        return -1;
    }

    socklen_t servlen = sizeof(servaddr);

    if(connect(sockfd, (struct sockaddr*)&servaddr, servlen) < 0)
    {
        printf("connect error: %s\n", strerror(errno));
        return -1;
    }

    char sendline[16] = "hello world";
    for(int i = 0; i < 2000; i++)
    {
        sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr*)&servaddr, servlen);
        sleep(1);
    }

    close(sockfd);

    return 0;
}