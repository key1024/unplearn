#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define	SERV_FIFO	"/tmp/fifo.serv"
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define MAXLINE 1024

int main(int argc, char **argv)
{
    /*create server's well-know FIFO; OK if already exists.*/
    if((mkfifo(SERV_FIFO, FILE_MODE) < 0) && (errno != EEXIST))
    {
        printf("can't create %s: %s\n", SERV_FIFO, strerror(errno));
        return -1;
    }

    int readfifo = open(SERV_FIFO, O_RDONLY, 0);
    if(readfifo < 0)
    {
        printf("open error: %s\n", strerror(errno));
        return -1;
    }
    int dummyfifo = open(SERV_FIFO, O_WRONLY, 0);
    if(dummyfifo < 0)
    {
        unlink(SERV_FIFO);
        printf("open error: %s\n", strerror(errno));
        return -1;
    }

    char buff[MAXLINE];
    ssize_t n = 0;
    while((n = read(readfifo, buff, MAXLINE)) > 0)
    {
        if(buff[n-1] == '\n')
            n--;
        buff[n] = '\0';

        char *ptr = NULL;
        if((ptr = strchr(buff, ' ')) == NULL)
        {
            printf("bogus request: %s\n", buff);
            continue;
        }

        *ptr++ = 0;
        pid_t pid = atol(buff);
        char fifoname[MAXLINE] = {0};
        snprintf(fifoname, MAXLINE, "/tmp/fifo.%ld", (long)pid);
        int writefifo = open(fifoname, O_WRONLY, 0);
        if(writefifo < 0)
        {
            printf("cannot open %s: %s\n", fifoname, strerror(errno));
            continue;
        }

        int fd = open(ptr, O_RDONLY);
        if(fd < 0)
        {
            snprintf(buff + n, MAXLINE-n, ": can't open, %s\n", strerror(errno));
            n = strlen(ptr);
            write(writefifo, ptr, n);
            close(writefifo);
        }
        else
        {
            while((n = read(fd, buff, MAXLINE)) > 0)
                write(writefifo, buff, n);

            close(fd);
            close(writefifo);
        }
    }

    return 0;
}