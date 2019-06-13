#include <stdio.h>
#include <pthread.h>

int main(void)
{
    while (1)
    {
        pthread_mutexattr_t mattr;
        pthread_mutexattr_init(&mattr);
    }

    return 0;
}