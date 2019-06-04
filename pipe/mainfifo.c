#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void client(int readfd, int writefd)
{
    char buff[1024];
    if(fgets(buff, sizeof(buff), stdin) == NULL)
    {
        printf("fgets error\n");
        return;
    }

    size_t len = strlen(buff);
    if(buff[len-1] == '\n')
        len--;

    write(writefd, buff, len);

    int n = 0;
    while((n = read(readfd, buff, sizeof(buff))) > 0)
    {
        write(STDOUT_FILENO, buff, n);
    }
}

void server(int readfd, int writefd)
{
    char buff[1024];
    ssize_t n = 0;

    if((n = read(readfd, buff, sizeof(buff))) <= 0)
    {
        printf("read pathname error\n");
        return;
    }
    buff[n] = '\0';

    int fd = open(buff, O_RDONLY);
    if(fd < 0)
    {
        snprintf(buff + n, sizeof(buff) - n, ": can't open, %s\n", strerror(errno));
        n = strlen(buff);
        write(writefd, buff, n);
    }
    else
    {
        while((n = read(fd, buff, sizeof(buff))) > 0)
        {
            write(writefd, buff, n);
        }
        close(fd);
    }
}

int main(int argc, char **argv)
{
    if((mkfifo(FIFO1, FILE_MODE) < 0) && (errno != EEXIST))
    {
        printf("can't create %s\n", FIFO1);
        return -1;
    }
    if((mkfifo(FIFO2, FILE_MODE) < 0) && (errno != EEXIST))
    {
        unlink(FIFO1);
        printf("can't create %s\n", FIFO2);
        return -1;
    }

    pid_t childpid;

    if((childpid = fork()) == 0)
    {
        int readfd = open(FIFO1, O_RDONLY, 0);
        int writefd = open(FIFO2, O_WRONLY, 0);

        server(readfd, writefd);

        close(readfd);
        close(writefd);
        return 0;
    }
    else
    {
        int writefd = open(FIFO1, O_WRONLY, 0);
        int readfd = open(FIFO2, O_RDONLY, 0);

        client(readfd, writefd);

        waitpid(childpid, NULL, 0);

        close(readfd);
        close(writefd);

        unlink(FIFO1);
        unlink(FIFO2);

        return 0;
    }
}