#include<stdio.h>
#include<mpi.h>

int main()
{
    int my_rank, comm_size;
    double a = 0.0, b = 5.0, local_a, local_b, h, n = 4000;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    h = (b - a)/n;
    local_a = a + 
}