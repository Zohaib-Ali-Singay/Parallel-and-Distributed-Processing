#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
int main()
{
    int rank, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = 20, *data = NULL;

    if (rank == 0)
    {
        data = (int*)malloc(n*sizeof(int));
        for (int i = 0; i < n; i++)
        {
            data[i] = i + 1;
        }
    }

    int local_n = n / size;
    int *local_data = (int*)malloc(local_n * sizeof(int));

    MPI_Scatter(data, local_n, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    int local_sum = 0, global_sum;
    for (int i = 0; i < local_n; i++)
    {
        local_sum += local_data[i];
    }

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Global Sum is %d\n", global_sum);
        free(data);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}