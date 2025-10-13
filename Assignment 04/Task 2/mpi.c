#include <mpi.h>    // MPI header for distributed processing
#include <stdio.h>  // Standard I/O functions
#include <stdlib.h> // Standard library (malloc, atoi, etc.)

int N;             // Total number of intervals for Pi approximation

double sum = 0.0;               // Global accumulator for partial sums

void compute_pi(int rank, int size);      // Function prototype for Pi computation

int main(int argc, char *argv[])
{
    // Parse command-line arguments
    N = strtol(argv[1], NULL, 10);          // Total slices for the series

    MPI_Init(&argc, &argv);  // Initialize the MPI environment

    int rank, size;          // Variables for current process rank and total processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get this process's rank
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get total number of processes

    // Compute local portion of Pi
    compute_pi(rank, size);

    // Process 0 collects and prints the final result
    if (rank == 0)
        printf("Value of pi is %f\n", 4.0 * sum);

    MPI_Finalize();

    return 0;  // End of program (MPI_Finalize missing but assumed)
}

// Function to compute partial sum of the Leibniz series for Pi
void compute_pi(int rank, int size)
{
    long my_rank = (long) rank;                // Convert rank to long
    double factor;                             // Alternating sign factor
    double my_sum = 0.0;                       // Local partial sum
    long i;
    long my_n = N / size;              // Number of slices per process
    long my_first = my_n * my_rank;            // Starting index for this process
    long my_last;                              // Ending index (exclusive)

    // Last process may take any remainder
    if (my_rank == size - 1)
        my_last = N;
    else
        my_last = my_first + my_n;

    // Determine initial sign based on starting index parity
    if (my_first % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    // Compute the local sum of the series terms
    for (i = my_first; i < my_last; i++)
    {
        my_sum += factor / (2 * i + 1);  // Add term to local sum
        factor = -factor;                 // Flip sign for next term
    }

    // Reduce all local sums into the global 'sum' on process 0
    MPI_Reduce(&my_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
}
