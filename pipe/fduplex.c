#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int fd[2];
    pid_t childpid;

    if(pipe(fd) != 0)
    {
        printf("pipe error: %s\n", strerror(errno));
        return -1;
    }

    if((childpid = fork()) == 0)
    {
        sleep(3);
        char c;
        read(fd[0], &c, 1);
        printf("child read %c\n", c);
        write(fd[0], "c", 1);
        return 0;
    }
    else
    {
        char c;
        write(fd[1], "p", 1);
        int n = 0; 
        while((n = read(fd[1], &c, 1))<0)
        {
            if(errno != EAGAIN)
            {
                printf("read error: %s\n", strerror(errno));
                break;
            }
        }
        
        printf("parent read %c length: %d\n", c, n);
        return 0;
    }
}