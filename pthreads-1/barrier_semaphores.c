#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>

sem_t count_sem, barrier_sem;
long thread_count, counter = 0;
void *thread_work(void * rank);

int main(int argc, char* argv[])
{
    long thread;
    thread_count = strtol(argv[1], NULL, 10);
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));

    sem_init(&count_sem, 0, 1);
    sem_init(&barrier_sem, 0, 0);

    for(thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, thread_work, (void *) thread);

    for(thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);

    free(thread_handles);

    return 0;
}

void *thread_work(void * rank)
{
    long my_rank = (long) rank;

    sem_wait(&count_sem);

    if(counter ==  thread_count - 1)
    {
        sem_post(&count_sem);

        for(long j = 0; j < (thread_count - 1); j++)
            sem_post(&barrier_sem);
    }

    else
    {
        counter++;
        sem_post(&count_sem);

        sem_wait(&barrier_sem);
    }

    printf("Thread %ld has reached\n", my_rank);

    return NULL;
}