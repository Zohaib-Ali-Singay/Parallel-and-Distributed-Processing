#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

long thread_count;
void *thread_work(void *rank);
long counter = 0;
pthread_cond_t cond_var;
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    long thread;
    thread_count = strtol(argv[1], NULL, 10);

    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));

    pthread_cond_init(&cond_var, NULL);
    pthread_mutex_init(&mutex, NULL);

    for(thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, thread_work, (void *)thread);

    for(thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);

    free(thread_handles);

    return 0;
}

void *thread_work(void *rank)
{
    pthread_mutex_lock(&mutex);

    counter++;

    if(counter == thread_count)
        pthread_cond_broadcast(&cond_var);

    else
        while(pthread_cond_wait(&cond_var, &mutex) != 0);

    pthread_mutex_unlock(&mutex);

    return NULL;
}