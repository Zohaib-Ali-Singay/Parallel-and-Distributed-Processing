#include<omp.h>
#include<stdio.h>
#include<stdlib.h>

long thread_count;
int main(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);
    int k, n = 1000;
    double factor;
    double pi;
    double sum = 0.0;
    # pragma omp parallel for reduction(+ : sum) private(factor)
    for (k = 0; k < n; k++)
    {
        if (k % 2 == 0)
            factor = 1.0;

        else
            factor = -1.0;

        sum += factor / ((2 * k) + 1);
    }
    pi = 4.0 * sum;
    printf("π = %f using solution 2\n", pi);

    /*factor = 1.0;
    # pragma omp parallel for num_threads (thread_count) reduction(+ : sum)
    for (k = 0; k < n; k++)
    {
        sum += factor / ((2 * k) + 1);
        factor = -factor;
    }
    pi = 4.0 * sum;
    printf("π = %f using solution 1\n", pi);*/
}