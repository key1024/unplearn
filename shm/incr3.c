#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

struct shared {
    sem_t mutex;
    int count;
}shared;

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("usage: incr1 <pathname> <#loops>\n");
        return -1;
    }

    int nloop = atoi(argv[2]);

    // int fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE);
    // write(fd, &shared, sizeof(struct shared));

    int fd = open("/dev/zero", O_RDWR);

    struct shared *ptr = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    sem_init(&ptr->mutex, 1, 1);

    setbuf(stdout, NULL);

    if(fork() == 0)
    {
        for(int i = 0; i < nloop; i++)
        {
            sem_wait(&ptr->mutex);
            printf("child: %d\n", ptr->count++);
            sem_post(&ptr->mutex);
        }

        exit(0);
    }

    for(int i = 0; i < nloop; i++)
    {
        sem_wait(&ptr->mutex);
        printf("parent: %d\n", ptr->count++);
        sem_post(&ptr->mutex);
    }

    return 0;
}