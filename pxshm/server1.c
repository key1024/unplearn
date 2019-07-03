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
    if(argc != 3)
    {
        printf("usage: server1 <shmname> <semname>\n");
        return -1;
    }

    shm_unlink(argv[1]);
    int fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
    ftruncate(fd, sizeof(struct shmstruct));
    struct shmstruct *ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    sem_unlink(argv[2]);
    mutex = sem_open(argv[2], O_CREAT | O_EXCL, FILE_MODE, 1);
    sem_close(mutex);

    return 0;
}