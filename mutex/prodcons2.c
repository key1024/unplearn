#include <stdio.h>
#include <pthread.h>

#define MAXNITEMS 1000000
#define MAXNTHREADS 100

int nitems = MAXNITEMS;
int buff[MAXNITEMS];

int index;

struct
{
    pthread_mutex_t mutex;
    int nput;
    int nval;
} put = {PTHREAD_MUTEX_INITIALIZER};

struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nready;
} nready = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

void *produce(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&put.mutex);
        if(put.nput >= nitems)
        {
            pthread_mutex_unlock(&put.mutex);
            return NULL;
        }

        buff[put.nput] = put.nval;
        put.nput++;
        put.nval++;
        pthread_mutex_unlock(&put.mutex);

        pthread_mutex_lock(&nready.mutex);
        if(nready.nready == 0)
        {
            pthread_cond_signal(&nready.cond);
            index++;
        }
        nready.nready++;
        pthread_mutex_unlock(&nready.mutex);

        *((int *)arg) += 1;
    }
    
}

void *consume(void *arg)
{
    for(int i = 0; i < nitems; i++)
    {
        pthread_mutex_lock(&nready.mutex);
        // 用while的原因是，当wait返回时再检查一次条件，防止出现虚假唤醒
        while(nready.nready == 0)
            pthread_cond_wait(&nready.cond, &nready.mutex);
        nready.nready--;
        pthread_mutex_unlock(&nready.mutex);

        if(buff[i] != i)
            printf("buff[%d] = %d\n", i, buff[i]);
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

    printf("index = %d\n", index);

    return 0;
}