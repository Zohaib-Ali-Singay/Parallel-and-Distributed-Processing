#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int N = 4;
int *matrix = NULL, *vector = NULL, *result = NULL;
long thread_count;
void *mat_mult(void *rank);
int main(int argc, char *argv[])
{
    long thread;
    thread_count = strtol(argv[1], NULL, 10);

    pthread_t* threads;

    matrix = (int*)malloc(N * N * sizeof(int));
    vector = (int*)malloc(N * sizeof(int));
    result = (int*)malloc(N * sizeof(int));
    threads = malloc(thread_count * sizeof(pthread_t));

    for (int i = 0; i < N; i++)
    {
        vector[i] = i + 1;
    }

    for(int i = 0; i < N * N; i++)
    {
        matrix[i] = i + 1;
    }

    for (thread = 0; thread < thread_count; thread ++)
    {
        pthread_create(&threads[thread], NULL, mat_mult, (void*) thread);
    }

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(threads[thread], NULL);
    }

    printf("Result is [");
    for(int i = 0; i < N; i++)
    {
        if (i == N - 1)
           printf("%d", result[i]);

        else
            printf("%d ", result[i]);
    }

    printf("]");
    printf("\n");

    // Clean up
    free(matrix);
    free(vector);
    free(result);
    free(threads);

    return 0;
}

void *mat_mult(void *rank)
{
    long my_rank = (long) rank;
    
    int local_n = N / thread_count;
    int my_first_row = local_n * my_rank;

    int my_last_row = (my_rank + 1) * local_n - 1;

    for (int i = my_first_row; i <= my_last_row; i++)
    {
        int sum = 0;
        for (int j = 0; j < N; j++)
        {
            sum += matrix[i * N + j] * vector[j];
        }
        result[i] = sum;
    }

    return NULL;
}