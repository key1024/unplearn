#include <stdio.h>
#include <pthread.h>

#define MAXNITEMS 1000000
#define MAXNTHREADS 100

struct {
    pthread_mutex_t mutex;
    int buff[MAXNITEMS];
    int nput;
    int nval;
} shared = {PTHREAD_MUTEX_INITIALIZER};

int nitems = MAXNITEMS;

void *produce(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&shared.mutex);
        if(shared.nput >= nitems)
        {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }

        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;
        pthread_mutex_unlock(&shared.mutex);
        *((int *)arg) += 1;
    }

    return NULL;
}

void consume_wait(int i)
{
    while (1)
    {
        pthread_mutex_lock(&shared.mutex);
        if(i < shared.nput)
        {
            pthread_mutex_unlock(&shared.mutex);
            return;
        }
        pthread_mutex_unlock(&shared.mutex);
    }
    
}

void *consume(void *arg)
{
    for(int i = 0; i < nitems; i++)
    {
        if(shared.buff[i] != i)
            printf("buff[%d] = %d\n", i, shared.buff[i]);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    int nthreads = MAXNTHREADS;
    int count[MAXNTHREADS];
    pthread_t tid_produce[MAXNTHREADS], tid_consume;
    for(int i = 0; i < nthreads; i++)
    {
        count[i] = 0;
        if(pthread_create(&tid_produce[i], NULL, produce, &count[i]) != 0)
        {
            printf("pthread_create error\n");
            return -1;
        }
    }
    pthread_create(&tid_consume, NULL, consume, NULL);

    int sum = 0;
    for(int i = 0; i < nthreads; i++)
    {
        pthread_join(tid_produce[i], NULL);
        printf("count[%d] = %d\n", i, count[i]);
        sum += count[i];
    }

    printf("sum = %d\n", sum);
    pthread_join(tid_consume, NULL);

    return 0;
}