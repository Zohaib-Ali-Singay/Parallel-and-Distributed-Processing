#include <stdio.h> 
#include <mpi.h> 
 
#define N 12  // Total number of elements (divisible by comm_sz) 
 
int main() { 
    int myrank, comm_sz; 
    double overall_block_to_cyclic_time, overall_cyclic_to_block_time; 
    MPI_Init(NULL, NULL); 
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 
 
    int block_size = N / comm_sz; 
    int block[block_size], cyclic[block_size]; 
    int original[N]; 
 
    if (myrank == 0) {// Initialize the vector with values 1 to N 
        for (int i = 0; i < N; i++) { 
            original[i] = i + 1; 
        } 
    } 
 
    //  Scatter data using block distribution 
    MPI_Scatter(original, block_size, MPI_INT, block, block_size, MPI_INT, 0, MPI_COMM_WORLD); 
 
    // Print block-distributed elements 
    printf("Process %d received block: ", myrank); 
    for (int i = 0; i < block_size; i++) { 
        printf("%d ", block[i]); 
    } 
    printf("\n"); 
 
    //  Convertion form Block to Cyclic 
 
    //Syncronization (start at same time) 
    MPI_Barrier(MPI_COMM_WORLD); 
    double start_time = MPI_Wtime();       //start time 
    for (int i = 0; i < block_size; i++) { 
        int send_to = (myrank + i) % comm_sz; 
        int recv_from = (myrank - i + comm_sz) % comm_sz; 
        MPI_Sendrecv(&block[i], 1, MPI_INT, send_to, 0, 
                     &cyclic[i], 1, MPI_INT, recv_from, 0, 
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
    } 
    double end_time = MPI_Wtime();          //end time
    double block_to_cyclic_time = end_time - start_time; 
     
    //  Take Maximum value for correct time  
    MPI_Reduce(&block_to_cyclic_time, &overall_block_to_cyclic_time ,1, MPI_DOUBLE, MPI_MAX, 
0, MPI_COMM_WORLD); 
     
    // Print cyclic-distributed elements 
    printf("Process %d received cyclic: ", myrank); 
    for (int i = 0; i < block_size; i++) { 
        printf("%d ", cyclic[i]); 
  } 
    printf("\n"); 
 
    //  Convertion form Cyclic to Block 
 
    //Syncronization (start at same time) 
    MPI_Barrier(MPI_COMM_WORLD); 
    start_time = MPI_Wtime();           //start time 
    for (int i = 0; i < block_size; i++) { 
        int send_to = (myrank - i + comm_sz) % comm_sz; 
        int recv_from = (myrank + i) % comm_sz; 
        MPI_Sendrecv(&cyclic[i], 1, MPI_INT, send_to, 1, 
                     &block[i], 1, MPI_INT, recv_from, 1, 
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
    } 
    end_time = MPI_Wtime();             //end time 
    double cyclic_to_block_time = end_time - start_time; 
 
    //  Take Maximum value for correct time  
    MPI_Reduce(&cyclic_to_block_time, &overall_cyclic_to_block_time ,1, MPI_DOUBLE, MPI_MAX, 
0, MPI_COMM_WORLD); 
     
    // Print final block elements 
    printf("Process %d restored block: ", myrank); 
    for (int i = 0; i < block_size; i++) { 
        printf("%d ", block[i]); 
    } 
    printf("\n"); 
     
    //  For correct order to display 
    MPI_Barrier(MPI_COMM_WORLD); 
     
    // Printing time measurements 
    if (myrank == 0) { 
        printf("Time taken for Block to Cyclic: %f seconds\n", overall_block_to_cyclic_time); 
        printf("Time taken for Cyclic to Block: %f seconds\n", overall_cyclic_to_block_time); 
    } 
 
    MPI_Finalize();
    return 0; 
}