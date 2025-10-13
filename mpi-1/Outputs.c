#include<stdio.h>
#include<mpi.h>
int main()
{
    int comm_size, my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    printf("Greetings from process %d of %d\n", my_rank, comm_size);
    
    MPI_Finalize();
    return 0;
}