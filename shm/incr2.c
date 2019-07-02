#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("usage: incr1 <pathname> <#loops>\n");
        return -1;
    }

    int nloop = atoi(argv[2]);

    int fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE);

    int zero = 0;
    write(fd, &zero, sizeof(int));

    int *ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    sem_t *mutex = sem_open("log", O_CREAT | O_EXCL, FILE_MODE, 1);
    sem_unlink("log");

    setbuf(stdout, NULL);

    if(fork() == 0)
    {
        for(int i = 0; i < nloop; i++)
        {
            sem_wait(mutex);
            printf("child: %d\n", (*ptr)++);
            sem_post(mutex);
        }

        exit(0);
    }

    for(int i = 0; i < nloop; i++)
    {
        sem_wait(mutex);
        printf("parent: %d\n", (*ptr)++);
        sem_post(mutex);
    }

    return 0;
}