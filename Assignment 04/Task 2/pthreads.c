#include <pthread.h>    // POSIX threads library
#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library (strtol, malloc, free)

int thread_count;       // Number of threads to create
int N;                  // Total number of terms for the series

double sum = 0.0;      // Shared accumulator for partial sums
pthread_mutex_t mutex;  // Mutex to protect concurrent updates to 'sum'

// Thread function prototype: each thread computes a portion of the series
void* compute_pi(void * rank);

int main(int argc, char *argv[])
{
    long thread;                // Loop variable for thread IDs
    pthread_t* threads;         // Array to hold thread handles

    // Initialize the mutex before any thread uses it
    pthread_mutex_init(&mutex, NULL);

    // Parse command-line arguments:
    // argv[1] = number of terms (N), argv[2] = number of threads
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <number_of_terms> <number_of_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    N = strtol(argv[1], NULL, 10);
    thread_count = strtol(argv[2], NULL, 10);

    // Allocate an array to store thread identifiers
    threads = malloc(thread_count * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Error allocating memory for threads\n");
        exit(EXIT_FAILURE);
    }

    // Create threads: each thread runs compute_pi with its rank as argument
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&threads[thread], NULL, compute_pi, (void*) thread);
    }

    // Wait for all threads to finish
    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(threads[thread], NULL);
    }

    // Clean up the mutex and dynamic memory
    pthread_mutex_destroy(&mutex);
    free(threads);

    // Print the final approximation of pi (Leibniz series sum times 4)
    printf("Value of pi is %f\n", 4.0 * sum);

    return 0;
}

// Thread function: compute partial sum of the Leibniz series
void *compute_pi(void *rank)
{
    long my_rank = (long) rank;   // Thread's rank (0 to thread_count-1)
    double factor;                // Alternating sign (+1 or -1)
    double my_sum = 0.0;          // Partial sum for this thread
    long i;

    // Determine number of terms per thread (integer division)
    long my_n = N / thread_count;
    // Compute starting index for this thread
    long my_first = my_n * my_rank;
    long my_last;                 // One past the last index for this thread

    // Last thread handles the remainder if N is not divisible by thread_count
    if (my_rank == thread_count - 1)
        my_last = N;
    else
        my_last = my_first + my_n;

    // Determine initial sign based on index parity
    if (my_first % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    // Compute partial sum: Leibniz series term by term
    // 
    // pi/4 = sum_{i=0 to infinity} [(-1)^i / (2*i + 1)]
    for (i = my_first; i < my_last; i++) {
        my_sum += factor / (2*i + 1);
        factor = -factor;  // Flip sign for next term
    }

    // Safely add this thread's partial sum to the global 'sum'
    pthread_mutex_lock(&mutex);
    sum += my_sum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}
