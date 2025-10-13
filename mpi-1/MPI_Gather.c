#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
int main()
{
    int rank, size, *local_buf = NULL, *gather_buf, send_count, recv_count;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    send_count = 5;
    recv_count = send_count;

    local_buf = (int*) malloc(send_count * sizeof(int));
    for (int i = 0; i < send_count; i++)
    {
        local_buf[i] = rank * send_count + i;
    }

    if (rank == 0)
    {
        gather_buf = (int*) malloc(recv_count * size * sizeof(int));
    }
    MPI_Gather(local_buf, send_count, MPI_INT, gather_buf, recv_count, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for(int i = 0; i < recv_count * size; i++)
        {
            printf("%d\n ", gather_buf[i]);
        }
    }

    printf("\n");
    free(local_buf);
    if (rank == 0) free(gather_buf);
    MPI_Finalize();
    return 0;
}