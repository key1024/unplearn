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
    int flags = O_RDWR | O_CREAT;
    int c;
    while ((c = getopt(argc, argv, "e")) != -1)
    {
        switch (c)
        {
        case 'e':
            flags |= O_EXCL;
            break;
        
        default:
            break;
        }
    }

    if(optind != argc - 2)
    {
        printf("usage: shmcreate [-e] <name> <length>\n");
        return -1;
    }

    off_t length = atoi(argv[optind + 1]);
    int fd = shm_open(argv[optind], flags, FILE_MODE);
    ftruncate(fd, length);
    
    char *ptr = mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    return 0;
}