#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>

// Global pointers for the full matrices and result (only allocated on root)
int *matrix1 = NULL, *result = NULL, *matrix2 = NULL, *col_matrixB = NULL;

// Function prototype to populate local portions of matrices
void *populate_matrix(int N, int rank, int local_rows, int size);

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);  // Initialize MPI environment

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get process rank
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get total number of processes

    // Read matrix dimension N from command-line argument
    int N = strtol(argv[1], NULL, 10);

    if (rank == 0)
        if(N % size != 0)
        {
            printf("Process counts must be a multiple of Matrix dimensions and must be less than or equal to dimension'\n");
            MPI_Finalize();
            return 1;
        }

    // Determine how many rows each process will handle
    int local_rows = N / size;
    if(rank == 0)
    {
        // On root, allocate full-size matrices and result
        matrix1 = (int*)malloc(N * N * sizeof(int));
        matrix2 = (int*)malloc(N * N * sizeof(int));
        result = (int*)malloc(N * N * sizeof(int));
    }

    // Allocate space for the transposed columns of B (full size)
    col_matrixB = (int *)malloc(N * N * sizeof(int));

    // Populate local segments of matrix1 and matrix2
    populate_matrix(N, rank, local_rows, size);

    if(rank == 0)
    {
        // On root, build the transposed version of matrix2 into col_matrixB
        int i = 0;
        for(int k = 0; k < N; k++)
        {
            for(int j = 0; j < N * N; j++)
            {
                if (j % N == 0)
                {
                    col_matrixB[i] = matrix2[k + j];
                    i++;
                }
            }
        }
    }

    // Broadcast the full transposed B matrix to all processes
    MPI_Bcast(col_matrixB, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate local storage for each process's chunk of A and result C
    int *local_matrix1 = (int *)malloc(N * (N / size) * sizeof(int));
    int *local_C = (int *)malloc((N / size) * N * sizeof(int));

    // Scatter rows of A among all processes
    MPI_Scatter(matrix1, N * (N / size), MPI_INT, local_matrix1, N * (N / size), MPI_INT, 0, MPI_COMM_WORLD);

    // Each process multiplies its local rows of A with the full B^T (col_matrixB)
    for (int i = 0; i < (N / size); i++)
    {
        for (int j = 0; j < N; j++) 
        {
            int sum = 0;
            for (int k = 0; k < N; k++)
            {
                // Multiply A(i,k) by B^T(j,k) and accumulate
                sum += local_matrix1[i * N + k] * col_matrixB[j * N + k];
            }

            // Store partial result
            local_C[i * N + j] = sum;
        }
    }

    // Gather all partial results back to root into 'result'
    MPI_Gather(local_C, (N / size) * N, MPI_INT, result, (N / size) * N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        // On root, print matrix A
        printf("Matrix A is \n");
        for (int i = 0; i < N * N ; i++)
        {
            if(i % N == 0 && i != 0)
                printf("\n");
            printf("%d ", matrix1[i]);
        }

        printf("\n\n");

        // Print matrix B
        printf("Matrix B is \n");
        for (int i = 0; i < N * N ; i++)
        {
            if(i % N == 0 && i != 0)
                printf("\n");
            printf("%d ", matrix2[i]);
        }

        // Print the final product
        printf("\n\nMatrix A x B:\n");
        if (N <= 5)
            for (int i = 0; i < N * N; i++)
            {
                if (i % N == 0) printf("\n");
                printf("%d ", result[i]);
            }
        
        else
        {
            printf("Matrix size is greater than 5 X 5, so we will print only first 5 X 5 elements");
            for (int i = 0; i < 5; i++)
            {
                printf("\n");
                for (int j = 0; j < 5; j++)
                {
                    printf("%d ", result[i * N + j]);  // print out only first 5 elements
                }
            }
        }


        printf("\n");

        // Free root's full-size buffers
        free(matrix1);
        free(matrix2);
        free(result);

    }
    // Free buffers allocated by all processes
    free(col_matrixB);
    MPI_Finalize();  // Finalize MPI environment
    return 0;
}

void *populate_matrix(int N, int rank, int local_rows, int size)
{
    // Seed the random number generator uniquely per process
    srand(time(NULL) + rank);

    int min = 1;
    int max = 20;

    // Allocate local buffers for each row segment
    int *local_matrix1 = (int *)malloc(N * (N / size) * sizeof(int));
    int *local_matrix2 = (int *)malloc(N * (N / size) * sizeof(int));

    // Fill local_matrix1 with random values
    for (int i = 0; i < (N * (N / size)); i++)
    {
        int random_in_range = (rand() % (max - min + 1)) + min;
        local_matrix1[i] = random_in_range;
    }

    // Reseed to get a different stream for matrix2
    srand(time(NULL) + rank + 1);

    // Fill local_matrix2 with random values
    for (int i = 0; i < (N * (N / size)); i++)
    {
        int random_in_range = (rand() % (max - min + 1)) + min;
        local_matrix2[i] = random_in_range;
    }

    // Gather local chunks into the global matrices on root
    MPI_Gather(local_matrix1, N * (N / size), MPI_INT, matrix1, N * (N / size), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(local_matrix2, N * (N / size), MPI_INT, matrix2, N * (N / size), MPI_INT, 0, MPI_COMM_WORLD);

    return NULL;
}
