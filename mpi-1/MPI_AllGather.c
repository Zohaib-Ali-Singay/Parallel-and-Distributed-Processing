#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Each process has 3 integers
    int count = 3;
    int *sendbuf = (int *)malloc(count * sizeof(int));
    int *recvbuf = (int *)malloc(size * count * sizeof(int));

    // Initialize the send buffer with values unique to each process
    // For example, process 0: 0, 1, 2; process 1: 10, 11, 12; etc.
    for (int i = 0; i < count; i++) {
        sendbuf[i] = rank * 10 + i;
    }

    // All processes gather data from each other
    MPI_Allgather(sendbuf, count, MPI_INT,
                  recvbuf, count, MPI_INT,
                  MPI_COMM_WORLD);

    // Print the gathered data from each process
    printf("Process %d received: ", rank);
    for (int i = 0; i < size * count; i++) {
        printf("%d ", recvbuf[i]);
    }
    printf("\n");

    // Clean up memory and finalize MPI
    free(sendbuf);
    free(recvbuf);
    MPI_Finalize();
    return 0;
}
