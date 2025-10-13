/*#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>

int main()
{
    int rank, size, N = 4;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *matrix = NULL, *vector, *results;

    if (rank == 0)
    {
        matrix = (int*)malloc(N * N * sizeof(int));
        vector = (int*)malloc(N * sizeof(int));
        results = (int*)malloc(N * sizeof(int));

        for(int i = 0; i < N * N; i++)
        {
            matrix[i] = i + 1;
        }

        for (int i = 0; i< N; i++)
        {
            vector[i] = i + 1;
        }
    }

    int local_no_rows = N / size;
    int *local_result = (int*)malloc(local_no_rows * sizeof(int));
    int *local_matrix = (int*)malloc(local_no_rows * N * sizeof(int));

    MPI_Scatter(matrix, local_no_rows * N, MPI_INT, local_matrix, local_no_rows * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(vector, N, MPI_INT, 0, MPI_COMM_WORLD);
    int sum;
    for (int i = 0; i < local_no_rows; i++)
    {
        sum = 0;
        for (int j = 0; j < N; j++)
        {
            sum = sum + local_matrix[(i * N) + j]*vector[j];
        }
        local_result[i] = sum;
    }

    MPI_Gather(local_result, local_no_rows, MPI_INT, results, local_no_rows, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Final Result of Matrix Multiplication is:\n");
        for (int i = 0; i < N; i++)
        {
            printf("%d\n", results[i]);
        }

        free(matrix);
        free(vector);
        free(results);
    }

    free(local_matrix);
    free(local_result);

    MPI_Finalize();
    return 0;

}*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Define matrix dimension
    int N = 4;

    // Pointers for matrix, vector, and result on the root
    int *matrix = NULL, *vector = NULL, *result = NULL;

    // Root process initializes the matrix and vector
    if (rank == 0) {
        matrix = (int*)malloc(N * N * sizeof(int));
        vector = (int*)malloc(N * sizeof(int));
        result = (int*)malloc(N * sizeof(int));

        // Fill the matrix and vector with some sample values
        for (int i = 0; i < N; i++) {
            vector[i] = i + 1;  // e.g., [1, 2, 3, 4]
        }
        for (int i = 0; i < N * N; i++) {
            matrix[i] = i + 1; // e.g., 1..16 in row-major order
        }
    }

    // Each process will handle 'local_rows' rows
    int local_rows = N / size;

    // Allocate space for the subset of the matrix each process receives
    int *local_matrix = (int*)malloc(local_rows * N * sizeof(int));

    // Scatter the rows of the matrix from root to all processes
    // Each process gets local_rows*N elements
    MPI_Scatter(matrix, local_rows * N, MPI_INT,
                local_matrix, local_rows * N, MPI_INT,
                0, MPI_COMM_WORLD);

    // Allocate space for the partial result (one entry per row)
    int *local_result = (int*)malloc(local_rows * sizeof(int));

    // Broadcast the entire vector to all processes
    if (rank == 0) {
        // already have vector
    }
    MPI_Bcast(vector, N, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process computes its portion of the matrix-vector product
    for (int i = 0; i < local_rows; i++) {
        int sum = 0;
        for (int j = 0; j < N; j++) {
            sum += local_matrix[i * N + j] * vector[j];
        }
        local_result[i] = sum;
    }

    // Gather the partial results from all processes into the 'result' array on root
    MPI_Gather(local_result, local_rows, MPI_INT,
               result, local_rows, MPI_INT,
               0, MPI_COMM_WORLD);

    // Root prints the final result vector
    if (rank == 0) {
        printf("Final result (Matrix × Vector) = ");
        for (int i = 0; i < N; i++) {
            printf("%d ", result[i]);
        }
        printf("\n");

        free(matrix);
        free(vector);
        free(result);
    }

    free(local_matrix);
    free(local_result);

    MPI_Finalize();
    return 0;
}
