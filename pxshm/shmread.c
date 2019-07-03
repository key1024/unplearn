#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <getopt.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: shmread <name>\n");
        return -1;
    }

    int fd = shm_open(argv[1], O_RDONLY, FILE_MODE);
    struct stat stat;
    fstat(fd, &stat);
    unsigned char *ptr = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    unsigned char c = 0;
    for (int i = 0; i < stat.st_size; i++)
    {
        printf("ptr[%d] = %d\n", i, c);

        if((c = *ptr++) != (i % 256))
        {
            printf("error: ptr[%d] = %d\n", i, c);
            return -1;
        }
    }

    return 0;
}