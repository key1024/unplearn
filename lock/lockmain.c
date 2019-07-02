#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SEQFILE "seqno"
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void my_lock(int fd)
{
    return;
}

void my_unlock(int fd)
{
    return;
}

int main(int argc, char **argv)
{
    pid_t pid = getpid();
    int fd = open(SEQFILE, O_RDWR, FILE_MODE);
    if(fd < 0)
    {
        printf("open error: %s\n", strerror(errno));
        return -1;
    }

    for(int i = 0; i < 20; i++)
    {
        my_lock(fd);

        lseek(fd, 0L, SEEK_SET);
        char line[16] = {0};
        ssize_t n = read(fd, line, 16);
        line[n] = '\0';
        long seqno = 0;
        sscanf(line, "%ld\n", &seqno);
        printf("%s: pid = %ld, seq# = %ld\n", argv[0], (long)pid, seqno);

        seqno++;
        snprintf(line, sizeof(line), "%ld\n", seqno);
        lseek(fd, 0L, SEEK_SET);
        write(fd, line, strlen(line));

        my_unlock(fd);
    }

    return 0;
}