#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        printf("usage: test1 <pathname> <filesize> <mmapsize>");
        return -1;
    }

    size_t filesize = atoi(argv[2]);
    size_t mmapsize = atoi(argv[3]);

    int fd = open(argv[1], O_RDWR | O_CREAT |O_TRUNC, FILE_MODE);
    lseek(fd, filesize-1, SEEK_SET);
    write(fd, "", 1);

    char *ptr = mmap(NULL, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    size_t pagesize = sysconf(_SC_PAGESIZE);
    printf("pagesize = %ld\n", (long)pagesize);

    int i = 0;
    for(i = 0; i < (filesize>mmapsize)?filesize:mmapsize; i += pagesize)
    {
        printf("ptr[%d] = %d\n", i, ptr[i]);
        ptr[i] = 1;
        printf("ptr[%d] = %d\n", i+pagesize-1, ptr[i+pagesize-1]);
        ptr[i+pagesize-1] = 1;
    }
    printf("prt[%d] = %d\n", i, ptr[i]);

    return 0;
}