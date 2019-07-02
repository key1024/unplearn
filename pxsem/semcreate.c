#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: semcreate <name>\n");
        return -1;
    }

    sem_t *sem = sem_open(argv[1], O_RDWR | O_CREAT, FILE_MODE, 5);
    if(sem == SEM_FAILED)
    {
        printf("sem_open %s failed: %s\n", argv[1], strerror(errno));
        return -1;
    }

    sem_close(sem);

    return 0;
}