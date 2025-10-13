#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
int main()
{
    int rank, size, *send_buf = NULL, *recv_buf, send_count, recv_count;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    send_count = 5;
    recv_count = send_count;

    if (rank == 0)
    {
        send_buf = (int*) malloc(size * send_count * sizeof(int));
        for (int i = 0; i < size * send_count; i++)
        {
            send_buf[i] = i;
        }
    }

    recv_buf = (int*) malloc(recv_count * sizeof(int));
    MPI_Scatter(send_buf, send_count, MPI_INT, recv_buf, recv_count, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Process %d is showing its array: \n", rank);
    for(int i = 0; i < recv_count; i++)
    {
        printf("%d ", recv_buf[i]);
    }
    printf("\n");
    free(recv_buf);
    if (rank == 0) free(send_buf);
    MPI_Finalize();
    return 0;
}