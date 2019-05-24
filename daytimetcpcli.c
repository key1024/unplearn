#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("Usage: a.out <IPAddress>\n");
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        printf("Create socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) != 1)
    {
        printf("inet_pton error, address: %s\n", argv[1]);
        close(sockfd);
        return -1;
    }

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error: %s\n", strerror(errno));
        close(sockfd);
        return -1;
    }

    int n = 0;
    char recvline[1024] = {0};
    while((n = read(sockfd, recvline, 1024-1)) > 0)
    {
        recvline[n] = 0;
        fputs(recvline, stdout);
    }

    if(n < 0)
    {
        printf("read error: %s\n", strerror(errno));
        close(sockfd);
        return -1;
    }

    close(sockfd);

    return 0;
}