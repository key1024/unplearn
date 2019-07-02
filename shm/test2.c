#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define FILE "test.data"
#define SIZE 32768
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{
    int fd = open(FILE, O_RDWR | O_CREAT | O_TRUNC, FILE_MODE);
    char *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    for(int i = 4096; i <= SIZE; i += 4096)
    {
        printf("setting file size to %d\n", i);
        ftruncate(fd, i);
        printf("ptr[%d] = %d\n", i-1, ptr[i-1]);
    }

    return 0;
}