#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <getopt.h>
#include <sys/wait.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: test3 <name>\n");
        return -1;
    }

    int fd1 = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
    ftruncate(fd1, sizeof(int));
    int fd2 = open("tmp", O_RDONLY);
    struct stat stat;
    fstat(fd2, &stat);

    pid_t childpid;
    int *ptr1, *ptr2;
    if((childpid = fork()) == 0)
    {
        ptr1 = mmap(NULL, sizeof(int), PROT_READ, MAP_SHARED, fd1, 0);
        ptr2 = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0);
        printf("child: shm ptr = %p, tmp ptr = %p\n", ptr1, ptr2);
        sleep(5);
        printf("shared memory interger = %d\n", *ptr1);
        return 0;
    }

    ptr2 = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd1, 0);
    ptr1 = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0);
    printf("parent: shm ptr = %p, tmp ptr = %p\n", ptr2, ptr1);
    *ptr2 = 777;
    waitpid(childpid, NULL, 0);

    shm_unlink(argv[1]);

    return 0;
}