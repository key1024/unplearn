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
#include <errno.h>
#include <string.h>
#include "cliserv2.h"

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        printf("usage: client2 <name> <#loops> <#usec>\n");
        return -1;
    }

    int nloop = atoi(argv[2]);
    int nusec = atoi(argv[3]);
    int fd = shm_open(argv[1], O_RDWR, FILE_MODE);
    shmstruct *ptr = mmap(NULL, sizeof(shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    pid_t pid = getpid();
    char mesg[MESGSIZE] = {0};
    for(int i = 0; i < nloop; i++)
    {
        sleep(nusec);
        snprintf(mesg, MESGSIZE, "pid %ld: message %d", (long)pid, i);

        if(sem_trywait(&ptr->nempty) == -1)
        {
            if(errno == EAGAIN)
            {
                sem_wait(&ptr->noverflowmutex);
                ptr->noverflow++;
                sem_post(&ptr->noverflowmutex);
                continue;
            }
            else
            {
                printf("sem_trywait error\n");
                return -1;
            }
        }
        sem_wait(&ptr->mutex);
        long offset = ptr->msgoff[ptr->nput];
        if(++(ptr->nput) >= NMESG)
            ptr->nput = 0;
        sem_post(&ptr->mutex);
        strcpy(&ptr->msgdata[offset], mesg);
        sem_post(&ptr->nstored);
    }

    return 0;
}