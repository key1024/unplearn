#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int count = 0;

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: incr1 <#loops>\n");
        return -1;
    }

    int nloop = atoi(argv[1]);

    sem_t *mutex = sem_open("log", O_CREAT | O_EXCL, FILE_MODE, 1);
    sem_unlink("log");

    setbuf(stdout, NULL);

    if(fork() == 0)
    {
        for(int i = 0; i < nloop; i++)
        {
            sem_wait(mutex);
            printf("child: %d\n", count++);
            sem_post(mutex);
        }

        exit(0);
    }

    for(int i = 0; i < nloop; i++)
    {
        sem_wait(mutex);
        printf("parent: %d\n", count++);
        sem_post(mutex);
    }

    return 0;
}