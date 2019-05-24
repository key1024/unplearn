#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

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

    while(1)
    {
        int connfd = accept(listenfd, NULL, NULL);

        time_t ticks = time(NULL);
        char buff[1024] = {0};
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));

        int nleft = strlen(buff);
        char *tmp = buff;
        while(1)
        {
            int nwrite = write(connfd, tmp, nleft);
            if(nwrite < 0)
            {
                if(errno == EINTR)
                    nwrite = 0;
                else
                {
                    printf("write error: %s\n", strerror(errno));
                    break;
                }
            }
            else if( nwrite == 0)
            {
                break;
            }

            nleft -= nwrite;
            tmp += nwrite;
        }

        close(connfd);
    }

    close(listenfd);

    return 0;
}