#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

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

    while(1)
    {
        char msg[1024] = {0};
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, msg, 1024, 0, (struct sockaddr*)&cliaddr, &len);
        if(n < 0)
        {
            printf("recvfrom error: %s\n", strerror(errno));
            break;
        }

        printf("recv msg from %s on %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

        n = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, len);
        if(n < 0)
        {
            printf("sendto error: %s\n", strerror(errno));
            break;
        }
    }

    close(sockfd);

    return 0;
}