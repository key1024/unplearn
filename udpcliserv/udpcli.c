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

    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) != 1)
    {
        printf("inet_pton error: %s\n", argv[1]);
        return -1;
    }

    socklen_t servlen = sizeof(servaddr);

    while(1)
    {
        char sendline[1024], recvline[1024];
        if(fgets(sendline, 1024, stdin) == NULL)
            break;

        int n = sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
        if(n < 0)
        {
            printf("sendto error: %s\n", strerror(errno));
            break;
        }

        socklen_t len = sizeof(cliaddr);
        n = recvfrom(sockfd, recvline, 1024, 0, (struct sockaddr*)&cliaddr, &len);
        if(n < 0)
        {
            printf("recvfrom errror: %s\n", strerror(errno));
            break;
        }

        if(len != servlen || memcmp(&servaddr, &cliaddr, len))
        {
            printf("reply from %s on %d(ignored)\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        }

        recvline[n] = 0;
        fputs(recvline, stdout);
    }

    close(sockfd);

    return 0;
}