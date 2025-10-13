#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

int thread_count;
long flag = 0;
double sum = 0.0;
pthread_mutex_t mutex;
void* compute_pi(void * rank);

int main(int argc, char *argv[])
{
    long thread;
    pthread_t* threads;

    pthread_mutex_init(&mutex, NULL);

    thread_count = strtol(argv[1], NULL, 10);
    threads = malloc(thread_count * sizeof(pthread_t));

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&threads[thread], NULL, compute_pi, (void*) thread);
    }

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(threads[thread], NULL);
    }

    pthread_mutex_destroy(&mutex);
    printf("Value of pi is %f\n", 4.0 * sum);

    return 0;
}

void *compute_pi(void *rank)
{
    long my_rank = (long) rank;
    double factor;
    double my_sum = 0.0;
    long i;
    long my_n = 1000 / thread_count; 
    long my_first = my_n * my_rank;
    long my_last = my_first + my_n;

    if (my_first % 2 == 0)
        factor = 1.0;

    else
        factor = -1.0;

    for (i = my_first; i < my_last; i++)
    {
        my_sum += factor / (2*i + 1);
        factor = -factor;
    }

    pthread_mutex_lock(&mutex);

    sum += my_sum;
    flag = (flag + 1) % thread_count;

    pthread_mutex_unlock(&mutex);
    return NULL;
}