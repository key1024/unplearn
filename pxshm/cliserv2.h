#ifndef _CLIENT_SERVER_2_H_
#define _CLIENT_SERVER_2_H_

#include <semaphore.h>

#define MESGSIZE    256
#define NMESG       16
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

typedef struct shmstruct {
    sem_t   mutex;
    sem_t   nempty;
    sem_t   nstored;
    int     nput;
    long    noverflow;
    sem_t   noverflowmutex;
    long    msgoff[NMESG];
    char    msgdata[NMESG*MESGSIZE];
}shmstruct;

#endif