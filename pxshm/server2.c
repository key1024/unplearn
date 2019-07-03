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
#include "cliserv2.h"

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: server2 <name>\n");
        return -1;
    }

    shm_unlink(argv[1]);
    int fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
    shmstruct *ptr = mmap(NULL, sizeof(shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ftruncate(fd, sizeof(shmstruct));
    close(fd);

    for (int i = 0; i < NMESG; i++)
    {
        ptr->msgoff[i] = i * MESGSIZE;
    }

    sem_init(&ptr->mutex, 1, 1);
    sem_init(&ptr->nempty, 1, NMESG);
    sem_init(&ptr->nstored, 1, 0);
    sem_init(&ptr->noverflowmutex, 1, 1);
    
    int index = 0;
    int lastnoverflow = 0;
    while(1)
    {
        sem_wait(&ptr->nstored);
        sem_wait(&ptr->mutex);
        long offset = ptr->msgoff[index];
        printf("index = %d: %s\n", index, &ptr->msgdata[offset]);
        if(++index >= NMESG)
            index = 0;
        sem_post(&ptr->mutex);
        sem_post(&ptr->nempty);

        sem_wait(&ptr->noverflowmutex);
        int temp = ptr->noverflow;
        sem_post(&ptr->noverflowmutex);
        if(temp != lastnoverflow)
        {
            printf("noverflow = %d\n", temp);
            lastnoverflow = temp;
        }
    }

    return 0;
}