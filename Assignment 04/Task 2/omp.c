#include <omp.h>   // OpenMP header for parallel directives and functions
#include <stdio.h>  // Standard I/O functions (printf)
#include <stdlib.h> // Standard library (strtol)

int thread_count;    // Number of threads to create
int N;               // Total number of terms in the Leibniz series

double sum = 0.0;    // Global accumulator for partial sums

void compute_pi();   // Function prototype for computing a slice of Pi

int main(int argc, char *argv[])
{
    // Parse command-line arguments: N (series length) and thread count
    N = strtol(argv[1], NULL, 10);
    thread_count = strtol(argv[2], NULL, 10);

    // Launch parallel region with specified number of threads
    #pragma omp parallel num_threads(thread_count)
        compute_pi();  // Each thread computes its portion

    // Only after all threads finish, print the final value of Pi
    printf("Value of pi is %f\n", 4.0 * sum);

    return 0;
}

// Each thread executes this function to compute its partial sum
void compute_pi()
{
    long my_rank = omp_get_thread_num();  // Thread ID (0 to thread_count-1)
    double factor;                        // Sign alternator for series terms
    double my_sum = 0.0;                  // Local sum for this thread
    long i;
    long my_n = N / thread_count;         // Number of terms per thread
    long my_first = my_n * my_rank;       // Starting index for this thread
    long my_last;                         // Ending index (exclusive)

    // Last thread takes any remaining terms if N not divisible exactly
    if (my_rank == thread_count - 1)
        my_last = N;
    else
        my_last = my_first + my_n;

    // Determine initial sign: +1 if starting index even, -1 if odd
    if (my_first % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    // Compute partial sum of Leibniz series: sum_{i=my_first}^{my_last-1} ((-1)^i / (2i + 1))
    for (i = my_first; i < my_last; i++)
    {
        my_sum += factor / (2 * i + 1);
        factor = -factor;  // Flip sign for next term
    }

    // Safely update the shared global sum
    #pragma omp critical
    sum += my_sum;
}
