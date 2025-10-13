#include<stdio.h>
#include<stdlib.h>
#include<cuda.h>
#include<curand_kernel.h>

// Global pointers for the two input matrices, the result, and the transposed B
int *matrix1 = NULL;
int *matrix2 = NULL;
int *result = NULL;

int num_blocks, num_threads, N;

 __global__ void populate_matrix(int *Md, int *Nd, int N)
{
    int Row = blockIdx.y * blockDim.y + threadIdx.y;
    int Col = blockIdx.x * blockDim.x + threadIdx.x;
    int idx = Row * N + Col;

    if((idx < N * N))
    {
        // Each thread gets its own curandState:
        //   curand_init(idx,    // 64-bit seed value (you choose)
        //               idx,     // sequence number = “offset” in random sequence
        //               0,       // offset within sequence
        //               &state); // pointer to our cuRAND state
        curandState state;
        curand_init(idx, idx, 0, &state);

        // Generate a random int in [0, UINT_MAX]:
        unsigned int r1 = curand(&state);
        unsigned int r2 = curand(&state);

        int min = 1, max = 20;

        Md[idx] = (r1 % (max - min + 1)) + min;

        Nd[idx] = (r2 % (max - min + 1)) + min;
    }
}

__global__ void MatrixMulKernel(int *Md, int *Nd, int *Pd, int N)
{
    int Row = blockIdx.y * blockDim.y + threadIdx.y;
    int Col = blockIdx.x * blockDim.x + threadIdx.x;

    int idx = Row * N + Col;
    if((idx < N * N))
    {
        int PValue = 0;
        for(int k = 0; k < N; k++)
        {
            int Md_element = Md[Row * N + k];
            int Nd_element = Nd[k * N + Col];

            PValue += (Md_element * Nd_element);
        }
        Pd[idx] = PValue;
    }
}

int main(int argc, char * argv[])
{
    N = strtol(argv[1], NULL, 10);
    num_blocks = strtol(argv[2], NULL, 10);
    num_threads = N * N;

    int num_threads_groups = (num_threads + 1023) / 1024; // Each block can contain a moximum of 1024 threads
    while(num_blocks < num_threads_groups)
    {
        num_blocks++;
    }

    while(num_blocks > num_threads_groups)
    {
        num_blocks--;
    }

    dim3 blocks(num_blocks, num_blocks);
    dim3 threads_per_block;
    if(num_threads <= 1024)
        threads_per_block = dim3(N, N);

    else
        threads_per_block = dim3(32, 32);


    matrix1 = (int*)malloc(N * N * sizeof(int));
    matrix2 = (int*)malloc(N * N * sizeof(int));
    result = (int*)malloc(N * N * sizeof(int));

    int *Md, *Nd, *Pd;
    int size = N * N * sizeof(int);
    
    cudaMalloc((void **) &Md, size);
    cudaMalloc((void **) &Nd, size);
    cudaMalloc((void **) &Pd, size);

    populate_matrix<<<blocks, threads_per_block>>>(Md, Nd, N);
    cudaDeviceSynchronize();

    MatrixMulKernel<<<blocks, threads_per_block>>>(Md, Nd, Pd, N);
    cudaDeviceSynchronize();

    cudaMemcpy(result, Pd, size, cudaMemcpyDeviceToHost);
    cudaMemcpy(matrix1, Md, size, cudaMemcpyDeviceToHost);
    cudaMemcpy(matrix2, Nd, size, cudaMemcpyDeviceToHost);

    cudaFree(Md);
    cudaFree(Nd);
    cudaFree(Pd);

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
    free(matrix1);
    free(matrix2);
    free(result);

    return(0);
}