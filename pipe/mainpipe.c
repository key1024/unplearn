#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

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
    int pipe1[2], pipe2[2];
    pid_t childpid;

    if(pipe(pipe1) != 0 || pipe(pipe2) != 0)
    {
        printf("pipe error: %s\n", strerror(errno));
        return -1;
    }
    
    if((childpid = fork()) == 0)
    {
        close(pipe1[1]);
        close(pipe2[0]);

        server(pipe1[0], pipe2[1]);

        return 0;
    }
    else
    {
        close(pipe1[0]);
        close(pipe2[1]);

        client(pipe2[0], pipe1[1]);

        waitpid(childpid, NULL, 0);
        return 0;
    }
}