#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // For simplicity, assume n is divisible by size
    int n = 16;
    int *data = NULL;

    // Root process initializes the array
    if (rank == 0) {
        data = (int*)malloc(n * sizeof(int));
        // Fill the array with some sample values (or random values)
        for (int i = 0; i < n; i++) {
            data[i] = rand() % 100; // random values between 0 and 99
        }
    }

    // Determine how many elements each process gets
    int local_n = n / size;
    int *local_data = (int*)malloc(local_n * sizeof(int));

    // Scatter the array among all processes
    MPI_Scatter(data, local_n, MPI_INT,
                local_data, local_n, MPI_INT,
                0, MPI_COMM_WORLD);

    // Each process finds its local maximum
    int local_max = INT_MIN;
    for (int i = 0; i < local_n; i++) {
        if (local_data[i] > local_max) {
            local_max = local_data[i];
        }
    }

    // Use MPI_Reduce to find the global maximum on the root
    int global_max = 0;
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    // The root process prints the final result
    if (rank == 0) {
        printf("Global maximum value = %d\n", global_max);
        free(data);
    }

    free(local_data);

    MPI_Finalize();
    return 0;
}
