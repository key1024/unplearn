#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define	PATH_PIDFILE	"pidfile"

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
    int pidfd = open(PATH_PIDFILE, O_RDWR | O_CREAT, FILE_MODE);
    if(write_lock(pidfd, 0, SEEK_SET, 0) < 0)
    {
        if(errno == EACCES || errno == EAGAIN)
        {
            printf("unable to lock %s, is %s already running?\n", PATH_PIDFILE, argv[0]);
            return -1;
        }
        else
        {
            printf("unable to lock %s\n", PATH_PIDFILE);
            return -1;
        }
        
    }

    char line[8] = {0};
    snprintf(line, sizeof(line), "%ld\n", (long)getpid());
    ftruncate(pidfd, 0);
    write(pidfd, line, strlen(line));

    pause();

    return 0;
}