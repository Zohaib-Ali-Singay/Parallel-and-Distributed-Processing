#include <stdio.h> 
#include <string.h> 
#include <time.h> 
#include <mpi.h> 
 
#define MAX_SIZE 100 
 
int main(){ 
    int myrank, comm_sz; 
    clock_t start_t, end_t; 
    double total_t; 
                                            //To verify answer i will use 4 buffers 
    char message1[] = "Hello from PING"; 
    char message1buffer[MAX_SIZE], message2buffer[MAX_SIZE]; 
    char message2[] = "Hello from PONG"; 
 
    MPI_Init(NULL, NULL); 
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 
 
    MPI_Barrier(MPI_COMM_WORLD);        //To SYNC 
    start_t = MPI_Wtime();              //Clocks at start of PING-PONG 
    if(myrank == 0){ 
        MPI_Send(message1, strlen(message1)+1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);   //Send PING 
        MPI_Recv(message2buffer, MAX_SIZE, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);      //Recieve PONG 
        //printf("%s\n", message2buffer);
    } 
    else{ 
        MPI_Recv(message1buffer, MAX_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);      //Receive PING 
        MPI_Send(message2, strlen(message2)+1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);                 //Send PONG 
        //printf("%s\n", message1buffer); 
    } 
    MPI_Barrier(MPI_COMM_WORLD);        //To SYNC 
    end_t = MPI_Wtime();                //Clocks at end of PING-PONG 
    if(myrank==0){ 
    total_t = (double)(end_t - start_t);       //Convert into seconds 
   printf("Total time taken by CPU: %f\n", total_t  ); 
    } 
} 