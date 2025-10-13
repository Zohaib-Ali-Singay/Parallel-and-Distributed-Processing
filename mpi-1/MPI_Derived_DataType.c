#include<stdio.h>
#include<mpi.h>
#include<string.h>
typedef struct
{
    int    intArr[3];
    double dblArr[2];
    char   charArr[5];
} MyData;

int main()
{
    int rank, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MyData sendData, recvData;

    int blocklengths[3] = {3, 2, 5};
    MPI_Datatype block_types[3] = {MPI_INT, MPI_DOUBLE, MPI_CHAR};

    MPI_Aint baseAddress, displacements[3];

    MyData temp;
    MPI_Get_address(&temp, &baseAddress);
    MPI_Get_address(&temp.intArr, &displacements[0]);
    MPI_Get_address(&temp.dblArr, &displacements[1]);
    MPI_Get_address(&temp.charArr, &displacements[2]);

    for (int i = 0; i < 3; i++) {
            displacements[i] -= baseAddress;
        }

    MPI_Datatype myDataType;

    MPI_Type_create_struct(
        3,
        blocklengths,
        displacements,
        block_types,
        &myDataType
    );

    MPI_Type_commit(&myDataType);

    if (rank == 0) {
        sendData.intArr[0] = 10;  sendData.intArr[1] = 20;  sendData.intArr[2] = 30;
        sendData.dblArr[0] = 3.14; sendData.dblArr[1] = 2.718;
        strcpy(sendData.charArr, "Hi!");

        printf("Rank 0 sending:\n");
        printf("  intArr  = [%d, %d, %d]\n",
               sendData.intArr[0], sendData.intArr[1], sendData.intArr[2]);
        printf("  dblArr  = [%f, %f]\n", sendData.dblArr[0], sendData.dblArr[1]);
        printf("  charArr = %s\n",       sendData.charArr);

        MPI_Send(&sendData, 1, myDataType, 1, 0, MPI_COMM_WORLD);
    }
    else if (rank == 1) {
        MPI_Recv(&recvData, 1, myDataType, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Rank 1 received:\n");
        printf("  intArr  = [%d, %d, %d]\n",
               recvData.intArr[0], recvData.intArr[1], recvData.intArr[2]);
        printf("  dblArr  = [%f, %f]\n", recvData.dblArr[0], recvData.dblArr[1]);
        printf("  charArr = %s\n",       recvData.charArr);
    }

    MPI_Type_free(&myDataType);
    MPI_Finalize();
    return 0;
}
