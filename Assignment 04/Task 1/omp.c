#include<omp.h>      // OpenMP header for parallel processing
#include<stdio.h>    // For input/output functions
#include<stdlib.h>   // For dynamic memory allocation and type conversions
#include<time.h>     // For random number generation based on time

// Global pointers for the full matrices and result
int *matrix1 = NULL, *result = NULL, *matrix2 = NULL, *col_matrixB = NULL;
int thread_count, N;  // Number of threads and matrix dimension

// Function declarations
void populate_matrix();
void mat_mul();

int main(int argc, char *argv[])
{
    // Read matrix dimension and number of threads from command line arguments
    N = strtol(argv[1], NULL, 10);
    thread_count = strtol(argv[2], NULL, 10);

    // Ensure matrix dimension is divisible by thread count
    if (N % thread_count != 0)
    {
        printf("Thread counts must be a multiple of Matrix dimensions'\n");
        return 1;
    }

    // Allocate memory for matrices and result
    matrix1 = (int*)malloc(N * N * sizeof(int));
    matrix2 = (int*)malloc(N * N * sizeof(int));
    result = (int*)malloc(N * N * sizeof(int));

    // Parallel matrix population
    # pragma omp parallel num_threads (thread_count)
    populate_matrix();

    // Allocate memory for transposed version of matrix2 (B)
    col_matrixB = (int *)malloc(N * N * sizeof(int));

    // Transpose matrix2 to optimize cache performance in multiplication
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

    // Parallel matrix multiplication
    # pragma omp parallel num_threads (thread_count)
    mat_mul();

    // Print matrix A
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

    // Print resulting matrix
    printf("\n\nMatrix A x B:\n");
    if (N <= 5)
    {
        for (int i = 0; i < N * N; i++)
        {
            if (i % N == 0) printf("\n");
            printf("%d ", result[i]);
        }
    }
    else
    {
        // Only show 5x5 portion if matrix is too large
        printf("Matrix size is greater than 5 X 5, so we will print only first 5 X 5 elements");
        for (int i = 0; i < 5; i++)
        {
            printf("\n");
            for (int j = 0; j < 5; j++)
            {
                printf("%d ", result[i * N + j]);
            }
        }
    }

    printf("\n");

    // Free allocated memory
    free(matrix1);
    free(matrix2);
    free(result);
    free(col_matrixB);

    return 0;
}

// Function to populate matrix1 and matrix2 with random values
void populate_matrix()
{
    int my_rank = omp_get_thread_num();  // Get current thread ID

    srand(time(NULL) + my_rank);         // Seed random generator differently for each thread

    int min = 1;
    int max = 20;

    int local_n = N / thread_count;         // Number of rows per thread
    int my_first_row = local_n * my_rank;   // Start row index
    int my_last_row = (my_rank + 1) * local_n - 1;  // End row index

    // Populate matrix1
    for (int i = my_first_row; i <= my_last_row; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int random_in_range = (rand() % (max - min + 1)) + min;
            matrix1[(i * N) + j] = random_in_range; 
        }
    }

    // Reseed for different random values in matrix2
    srand(time(NULL) + my_rank + 1);

    // Populate matrix2
    for (int i = my_first_row; i <= my_last_row; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int random_in_range = (rand() % (max - min + 1)) + min;
            matrix2[(i * N) + j] = random_in_range; 
        }
    }
}

// Function to perform matrix multiplication
void mat_mul()
{
    int my_rank = omp_get_thread_num();  // Get thread ID

    int local_n = N / thread_count;          // Rows per thread
    int my_first_row = local_n * my_rank;    // First row to process
    int my_last_row = (my_rank + 1) * local_n - 1;  // Last row to process

    // Multiply matrix1 by transposed matrix2 (col_matrixB)
    for (int i = my_first_row; i <= my_last_row; i++)
    {
        for (int j = 0; j < N; j++) 
        {
            int sum = 0;
            for (int k = 0; k < N; k++)
            {
                // A[i][k] * B[k][j] using col_matrixB
                sum += matrix1[i * N + k] * col_matrixB[j * N + k];
            }

            // Store result
            result[i * N + j] = sum;
        }
    }
}
