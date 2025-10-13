#include<omp.h>
#include<stdio.h>
#include<stdlib.h>

long thread_count;

int main(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);
    long phase;

    int i, temp;
    int a[10] = {6, 9, 1, 5, 8, 10, 2, 4, 7, 3};
    int n = 10;
    # pragma omp parallel num_threads (thread_count) private(i, temp, phase)
    for(phase = 0; phase < n; phase++)
    {
        if(phase % 2 == 0)
        {
            # pragma omp for
            for(i = 1; i < n; i+=2)
            {
                if(a[i - 1] > a[i])
                {
                    temp = a[i - 1];
                    a[i - 1] = a[i];
                    a[i] = temp;
                }
            }
        }

        else
        {
            # pragma omp for
            for(i = 1; i < n - 1; i += 2)
            {
                if(a[i] > a[i + 1])
                {
                    temp = a[i + 1];
                    a[i + 1] = a[i];
                    a[i] = temp;
                }
            }
        }
    }

    printf("Sorted array is [ ");
    for(i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("]\n");

    return 0;
}