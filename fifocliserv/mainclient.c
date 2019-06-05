#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define	SERV_FIFO	"/tmp/fifo.serv"
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define MAXLINE 1024

int main(int argc, char **argv)
{
    pid_t pid = getpid();

    char fifoname[MAXLINE] = {0};
    snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long)pid);
    if((mkfifo(fifoname, FILE_MODE) < 0) && (errno != EEXIST))
    {
        printf("can't create %s\n", fifoname);
        return -1;
    }

    char buff[MAXLINE] = {0};
    snprintf(buff, sizeof(buff), "%ld ", (long)pid);
    size_t len = strlen(buff);
    char *ptr = buff + len;

    if(fgets(ptr, MAXLINE-len, stdin) == NULL)
    {
        printf("fgets error\n");
        return -1;
    }
    len = strlen(buff);

    int writefifo = open(SERV_FIFO, O_WRONLY, 0);
    write(writefifo, buff, len);
    int readfifo = open(fifoname, O_RDONLY, 0);
    int n = 0;
    while ((n = read(readfifo, buff, MAXLINE)) > 0)
    {
        write(STDOUT_FILENO, buff, n);
    }

    close(readfifo);
    close(writefifo);
    unlink(fifoname);
    
    return 0;
}