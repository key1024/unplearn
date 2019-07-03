#include <stdio.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: shmunlink <name>\n");
        return -1;
    }

    shm_unlink(argv[1]);

    return 0;
}