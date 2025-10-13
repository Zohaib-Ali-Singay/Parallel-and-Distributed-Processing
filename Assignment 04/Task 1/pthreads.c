#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global pointers for the two input matrices, the result, and the transposed B
int *matrix1 = NULL;
int *matrix2 = NULL;
int *col_matrixB = NULL;
int *result = NULL;

int thread_count;  // Number of threads to use
int N;             // Dimension of the (square) matrices

// Thread function prototypes
void *populate_matrix(void *rank);
void *mat_mul(void *rank);

int main(int argc, char* argv[])
{
    // Parse command‑line arguments: matrix size and number of threads
    N = strtol(argv[1], NULL, 10);
    thread_count = strtol(argv[2], NULL, 10);

    // Ensure we can evenly divide rows among threads
    if (N % thread_count != 0)
    {
        printf("Error: thread count must evenly divide matrix dimension\n");
        return 1;
    }

    // Allocate space for matrices and thread handles
    matrix1   = malloc(N * N * sizeof(int));
    matrix2   = malloc(N * N * sizeof(int));
    result    = malloc(N * N * sizeof(int));
    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));

    // ---------- Phase 1: Populate matrices in parallel ----------
    for (int t = 0; t < thread_count; t++)
    {
        pthread_create(&threads[t], NULL, populate_matrix, (void*)(long)t);
    }
    // Wait for all populate_matrix threads to finish
    for (int t = 0; t < thread_count; t++)
    {
        pthread_join(threads[t], NULL);
    }

    // ---------- Prepare transposed B (col_matrixB) for cache-friendly access ----------
    col_matrixB = malloc(N * N * sizeof(int));

    int idx = 0;
    // Walk each column of matrix2 by scanning rows in the flat array
    for (int col = 0; col < N; col++)
    {
        for (int base = 0; base < N * N; base += N)
        {
            // base + col is element (base/N)-th row, col-th column
            col_matrixB[idx++] = matrix2[base + col];
        }
    }

    // ---------- Phase 2: Multiply matrices in parallel ----------
    for (long t = 0; t < thread_count; t++)
    {
        pthread_create(&threads[t], NULL, mat_mul, (void*)t);
    }

    // Wait for all mat_mul threads to finish
    for (long t = 0; t < thread_count; t++)
    {
        pthread_join(threads[t], NULL);
    }

    // ---------- Print Matrix A ----------
    printf("Matrix A:\n");
    for (int i = 0; i < N * N; i++)
    {
        if (i % N == 0) printf("\n");
        printf("%d ", matrix1[i]);
    }
    printf("\n\n");

    // ---------- Print Matrix B ----------
    printf("Matrix B:\n");
    for (int i = 0; i < N * N; i++)
    {
        if (i % N == 0) printf("\n");
        printf("%d ", matrix2[i]);
    }
    printf("\n\n");

    // ---------- Print Result A × B ----------
    printf("Result (A X B):\n");
    if (N <= 5)
    {
        // Print entire result if small
        for (int i = 0; i < N * N; i++)
        {
            if (i % N == 0)
                printf("\n");
            printf("%d ", result[i]);
        }
    }

    else
    {
        // Otherwise print only the top-left 5×5 block
        printf("Showing top-left 5 X 5 block only:\n");
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                printf("%d ", result[i * N + j]);
            }
            printf("\n");
        }
    }
    printf("\n");

    // ---------- Cleanup ----------
    free(matrix1);
    free(matrix2);
    free(col_matrixB);
    free(result);
    free(threads);

    return 0;
}

// Thread function to fill matrix1 and matrix2 with random values
void *populate_matrix(void *rank)
{
    long my_rank = (long)rank;

    // Unique seed per thread to avoid identical sequences
    srand(time(NULL) + my_rank);

    int local_rows = N / thread_count;
    int start_row = my_rank * local_rows;
    int end_row   = start_row + local_rows - 1;
    int min = 1, max = 20;

    // Fill assigned rows of matrix1
    for (int i = start_row; i <= end_row; i++)
    {
        for (int j = 0; j < N; j++) 
        {
            matrix1[i * N + j] = (rand() % (max - min + 1)) + min;
        }
    }

    // Reseed for matrix2 to get different values
    srand(time(NULL) + my_rank + 1);

    // Fill assigned rows of matrix2
    for (int i = start_row; i <= end_row; i++)
    {
        for (int j = 0; j < N; j++) 
        {
            matrix2[i * N + j] = (rand() % (max - min + 1)) + min;
        }
    }

    return NULL;
}

// Thread function to compute a slice of the result matrix
void *mat_mul(void *rank)
{
    long my_rank = (long)rank;

    int local_rows = N / thread_count;
    int start_row = my_rank * local_rows;
    int end_row   = start_row + local_rows - 1;

    // For each assigned row of A, compute full row of A×B
    for (int i = start_row; i <= end_row; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            int sum = 0;
            // Dot product: row i of A with column j of B (accessed via col_matrixB)
            for (int k = 0; k < N; k++) 
            {
                sum += matrix1[i * N + k] * col_matrixB[j * N + k];
            }
            result[i * N + j] = sum;
        }
    }

    return NULL;
}
