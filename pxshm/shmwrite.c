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
        printf("usage: shmwrite <name>\n");
        return -1;
    }

    int fd = shm_open(argv[1], O_RDWR, FILE_MODE);
    struct stat stat;
    fstat(fd, &stat);
    unsigned char *ptr = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    for(int i = 0; i < stat.st_size; i++)
    {
        *ptr++ = i % 256;
    }

    return 0;
}