#include <stdio.h>
#include <semaphore.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: semgetvalue <name>\n");
        return -1;
    }

    sem_t *sem = sem_open(argv[1], 0);
    int val = 0;
    sem_getvalue(sem, &val);
    printf("value = %d\n", val);

    return 0;
}