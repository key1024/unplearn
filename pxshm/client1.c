#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <getopt.h>
#include <sys/wait.h>
#include <semaphore.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

struct shmstruct {
    int count;
};

sem_t *mutex;

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        printf("usage: client1 <shmname> <semname> <#loops>\n");
        return -1;
    }
    int nloop = atoi(argv[3]);
    int fd = shm_open(argv[1], O_RDWR, FILE_MODE);
    struct shmstruct *ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    mutex = sem_open(argv[2], 0);
    pid_t pid = getpid();
    for(int i = 0; i < nloop; i++)
    {
        sem_wait(mutex);
        printf("pid %ld: %d\n", (long)pid, ptr->count++);
        sem_post(mutex);
    }

    return 0;
}