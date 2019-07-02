#include <stdio.h>
#include <semaphore.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: semunlink <name>\n");
        return -1;
    }

    sem_unlink(argv[1]);

    return 0;
}