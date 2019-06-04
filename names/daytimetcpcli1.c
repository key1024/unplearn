#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("usage: daytimetcpcli1 <hostname> <service>\n");
        return -1;
    }

    // 通过hostname获取IP地址
    struct in_addr **pptr;
    struct in_addr *inetaddrp[2];
    struct in_addr inetaddr;
    struct hostent *hp = gethostbyname(argv[1]);
    if(hp == NULL)
    {
        if(inet_aton(argv[1], &inetaddr) == 0)
        {
            printf("hostname error for %s: %s\n", argv[1], hstrerror(h_errno));
            return -1;
        }
        else
        {
            inetaddrp[0] = &inetaddr;
            inetaddrp[1] = NULL;
            pptr = inetaddrp;
        }
    }
    else
    {
        pptr = (struct in_addr**)hp->h_addr_list;
    }

    struct servent *sp = getservbyname(argv[2], "tcp");
    if(sp == NULL)
    {
        printf("getservbyname error for %s\n", argv[2]);
        return -1;
    }

    int sockfd = -1;
    for(; *pptr != NULL; ++pptr)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in servaddr;
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = sp->s_port;
        memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
        printf("trying %s:%d\n", inet_ntoa(servaddr.sin_addr), htons(servaddr.sin_port));

        if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == 0)
            break;
        printf("connect error\n");
        close(sockfd);
    }

    if(*pptr == NULL)
    {
        printf("unable to connect\n");
        return -1;
    }

    int n = 0;
    char recvline[1024] = {0};
    while((n = read(sockfd, recvline, 1024)) > 0)
    {
        recvline[n] = 0;
        fputs(recvline, stdout);
    }
    
    return 0;
}