#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<semaphore.h>

sem_t *semaphores = NULL;
long thread_count;
void* send_message(void * rank);
char **messages = NULL;
int main(int argc, char *argv[])
{
    long thread;
    thread_count = strtol(argv[1], NULL, 10);

    pthread_t *thread_handles;

    thread_handles = malloc(thread_count * sizeof(pthread_t));
    messages = (char **)malloc(thread_count * sizeof(char*));
    semaphores = malloc(thread_count * sizeof(sem_t));

    

    for(thread = 0; thread < thread_count; thread++)
    {
        sem_init(&semaphores[thread], 0, 0);
    }

    for(thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, send_message, (void *) thread);
    }

    for(thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }

    for(thread = 0; thread < thread_count; thread++)
    {
        sem_destroy(&semaphores[thread]);
    }
    

    return 0;
}

void *send_message(void *rank)
{
    long my_rank = (long) rank;
    long dest = (my_rank + 1) % thread_count;
    //long source = (my_rank + thread_count - 1) % thread_count;
    char *message = malloc(100 * sizeof(char));

    sprintf(message, "Hello to %ld from %ld", dest, my_rank);
    messages[dest] = message;
    
    sem_post(&semaphores[dest]);

    sem_wait(&semaphores[my_rank]);

    printf("Thread %ld > %s\n", my_rank, messages[my_rank]);

    return NULL;
}