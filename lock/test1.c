#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;
    lock.l_type = type;     /*F_RDLCK, FWRLCK, F_UNLCK*/
    lock.l_start = offset;  /* byte offset, relative to l_whence */
    lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len;       /* #bytes (0 means to EOF) */

    return (fcntl(fd, cmd, &lock));     /* -1 upon error */
}

#define	read_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)
#define	readw_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLKW, F_RDLCK, offset, whence, len)
#define	write_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)
#define	writew_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLKW, F_WRLCK, offset, whence, len)
#define	un_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLK, F_UNLCK, offset, whence, len)
#define	is_read_lockable(fd, offset, whence, len) \
			lock_test(fd, F_RDLCK, offset, whence, len)
#define	is_write_lockable(fd, offset, whence, len) \
			lock_test(fd, F_WRLCK, offset, whence, len)

int main(int argc, char **argv)
{
    int fd = open("test.data", O_RDWR | O_CREAT, FILE_MODE);

    read_lock(fd, 0, SEEK_SET, 0);
    printf("parent has read lock\n");

    if(fork() == 0)
    {
        sleep(1);
        printf("first child tries to obtain write lock.\n");
        writew_lock(fd, 0, SEEK_SET, 0);
        printf("first child obtains write lock.\n");
        sleep(2);
        un_lock(fd, 0, SEEK_SET, 0);
        printf("first child releases write lock.\n");
        exit(0);
    }

    if(fork() == 0)
    {
        sleep(3);
        printf("second child tries to obtain read lock.\n");
        readw_lock(fd, 0, SEEK_SET, 0);
        printf("second child obtains read lock.\n");
        sleep(4);
        un_lock(fd, 0, SEEK_SET, 0);
        printf("second child releases read lock.\n");
        exit(0);
    }

    sleep(5);
    un_lock(fd, 0, SEEK_SET, 0);
    printf("parent releases read lock.\n");

    return 0;
}