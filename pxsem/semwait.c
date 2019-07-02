#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: semwait <name>\n");
        return -1;
    }

    sem_t *sem = sem_open(argv[1], 0);
    sem_wait(sem);
    int val = 0;
    sem_getvalue(sem, &val);
    printf("pid %ld has semphore, value = %d\n", (long)getpid(), val);

    pause();

    return 0;
}