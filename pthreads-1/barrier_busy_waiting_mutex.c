#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

pthread_mutex_t barrier_mutex;
void *thread_work(void *rank);
long thread_count;
long counter = 0;
long flag = 0;
int main(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);

    long thread;
    pthread_t *thread_handles = NULL;
    thread_handles = malloc(thread_count * sizeof(pthread_t));

    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, thread_work, (void*) thread);

    for(thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);

    pthread_mutex_destroy(&barrier_mutex);

    free(thread_handles);

    return 0;
}

void *thread_work(void * rank)
{
    long my_rank = (long) rank;
    pthread_mutex_lock(&barrier_mutex);
    counter++;
    pthread_mutex_unlock(&barrier_mutex);

    while(counter < thread_count);

    if(my_rank == 0)
        printf("All processes have been synchronized\n");

    while(flag != my_rank);

    printf("Thread %ld activated\n", my_rank);
    flag = (flag + 1) % thread_count;

    return NULL;
}