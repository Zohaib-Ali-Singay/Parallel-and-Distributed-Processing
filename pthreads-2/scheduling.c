#include<omp.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

long thread_count;
double f(int i)
{
    int j, start = (i * (i + 1)) / 2, finish = start + i;
    double return_val = 0.0;

    for(j = start; j <= finish; j++)
        return_val += sin(j);

    return return_val;
}
int main(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);

    double start_time, end_time, sum = 0.0;
    int i;

    start_time = omp_get_wtime();

    # pragma omp parallel for num_threads (thread_count) reduction(+ : sum)
    for(i = 0; i <= 10000; i++)
    {
        sum += f(i);
    }

    end_time = omp_get_wtime();

    printf("Without scheduling, execution time is %f\n", end_time - start_time);

    sum = 0.0;
    start_time = omp_get_wtime();

    # pragma omp parallel for num_threads (thread_count) reduction(+ : sum) schedule(static, 1)
    for(i = 0; i <= 10000; i++)
    {
        sum += f(i);
    }

    end_time = omp_get_wtime();

    printf("With schedule(static, 1), execution time is %f\n", end_time - start_time);

    sum = 0.0;
    start_time = omp_get_wtime();

    # pragma omp parallel for num_threads (thread_count) reduction(+ : sum) schedule(dynamic, 10)
    for(i = 0; i <= 10000; i++)
    {
        sum += f(i);
    }

    end_time = omp_get_wtime();

    printf("With schedule(dynamic, 10), execution time is %f\n", end_time - start_time);

    sum = 0.0;
    start_time = omp_get_wtime();

    # pragma omp parallel for num_threads (thread_count) reduction(+ : sum) schedule(guided, 2)
    for(i = 0; i <= 10000; i++)
    {
        sum += f(i);
    }

    end_time = omp_get_wtime();

    printf("With schedule(guided, 2), execution time is %f\n", end_time - start_time);

    return 0;
}