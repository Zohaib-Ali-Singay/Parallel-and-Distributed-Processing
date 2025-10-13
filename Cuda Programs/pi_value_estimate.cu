#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Global variables to hold the number of blocks and the number of terms (N)
int num_blocks;
int N;

// CUDA kernel to compute a partial sum for pi approximation using the Leibniz series
__global__ void compute_pi(int num_blocks, int N, float *sum)
{
    double factor;

    // Compute the global row and column indices for this thread
    int Row = blockIdx.y * blockDim.y + threadIdx.y;
    int Col = blockIdx.x * blockDim.x + threadIdx.x;

    // Flatten 2D indices into a 1D index
    int idx = Row * N + Col;

    // Determine the sign of the term: +1 for even idx, -1 for odd idx
    if (idx % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    // Each thread computes one term of the series if within bounds
    if (idx < N)
    {
        // Leibniz term: factor / (2*idx + 1)
        float term = factor / (2 * idx + 1);
        // Atomically add the term to the shared sum variable
        atomicAdd(sum, term);
    }
}

int main(int argc, char *argv[])
{
    // Host variable to store the sum of series
    float sum = 0.0f;

    // Parse command-line arguments: N (number of terms) and num_blocks (desired blocks)
    N = strtol(argv[1], NULL, 10);
    num_blocks = strtol(argv[2], NULL, 10);

    // Calculate the number of thread groups needed (each block can have up to 1024 threads)
    int num_threads_groups = (N + 1023) / 1024;

    // Adjust num_blocks to match the number of thread groups
    while (num_blocks < num_threads_groups)
    {
        num_blocks++;
    }
    while (num_blocks > num_threads_groups)
    {
        num_blocks--;
    }

    // Define the block and thread dimensions for kernel launch
    dim3 blocks(num_blocks);
    dim3 threads_per_block;

    // If N is small, use a square root decomposition for threads per block
    if (N <= 1024)
        threads_per_block = dim3((int)sqrt(N), (int)sqrt(N));
    else
        // Otherwise, use a 32x32 block of threads
        threads_per_block = dim3(32, 32);

    // Allocate device memory for the sum
    float *d_sum;
    cudaMalloc((void**)&d_sum, sizeof(float));
    
    // Initialize device sum to zero
    cudaMemset(d_sum, 0, sizeof(float));

    // Launch the kernel to compute the series
    compute_pi<<<blocks, threads_per_block>>>(num_blocks, N, d_sum);

    // Copy the computed sum back to host
    cudaMemcpy(&sum, d_sum, sizeof(float), cudaMemcpyDeviceToHost);

    // Multiply by 4 to get the approximation for pi and print the result
    printf("Value of pi is %f\n", 4.0 * sum);

    return 0;
}
