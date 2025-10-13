#include <stdio.h>
#include <mpi.h>
#include<string.h>
const int MAX_STRING = 100;
int main()
{
    int my_rank, comm_size, count_p;
    MPI_Status status;
    char greetings[MAX_STRING];

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(my_rank != 0)
    {
        sprintf(greetings, "Greetings from process %d of %d", my_rank, comm_size);
        MPI_Send(greetings, strlen(greetings)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    else
    {
        printf("Greetings from process %d of %d\n", my_rank, comm_size);
        for (int i = 1; i < comm_size; i++)
        {
            MPI_Recv(greetings, MAX_STRING, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
            printf("%d, %d\n", status.MPI_SOURCE, status.MPI_TAG);
            MPI_Get_count(&status, MPI_CHAR, &count_p);
            printf("%d\n", count_p);
            /*MPI_Get_Count()*/
            printf("%s\n", greetings);
        }
    }

    MPI_Finalize();
    return 0;
}
