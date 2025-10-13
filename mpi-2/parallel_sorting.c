#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_Comm_WORLD, &rank);

    int a[16] = [15, 11, 9, 16, 3, 14, 8, 7, 4, 6, 12, 10, 5, 2, 13, 1];
    int local_a[4];

    MPI_Scatter(a,   /* send buffer (only significant at root) */
        4,        /* number of elements sent to each process */
        MPI_INT,        /* data type of send buffer */
        local_a,    /* receive buffer */
        4,        /* number of elements received */
        MPI_INT,        /* data type of receive buffer */
        0,              /* root rank */
        MPI_COMM_WORLD)

    MPI_Finalize();

    return 0;
}