#include<stdio.h>
#include<mpi.h>
#include<math.h>
double Trapezoidal_Area(double local_a, double local_b, int local_n, double h);
double function(double a);
int main()
{
    double a = 0.0, b = 4.0, local_int, total_int_reduce, h, local_a, local_b, total_int_all_reduce; 
    int local_n, comm_size, my_rank, n = 24000, c;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    h = (b-a)/n;
    local_n = n / comm_size;
    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;

    local_int = Trapezoidal_Area(local_a, local_b, local_n, h);

    MPI_Reduce(&local_int, &total_int_reduce, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (my_rank == 0)
    {
        c = 2;
        printf("With MPI_Reduce, Total Area is %f\n", total_int_reduce);
    }
    MPI_Bcast(&c, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Allreduce(&local_int, &total_int_all_reduce, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (my_rank != 0)
    {
        printf("With MPI_Allreduce, Total Area is %f\n", total_int_all_reduce);
        printf("Value of C is %d\n", c);
    }
    MPI_Finalize();
    return 0;
}

double function(double a)
{
    return ((a*a*a) + 3*(a*a) + 2);
}

double Trapezoidal_Area(double local_a, double local_b, int local_n, double h)
{
    double estimate, x;
    estimate = (function(local_a) + function(local_b))/2.0;

    for (int i = 1; i <= local_n - 1; i++)
    {
        x = local_a + (i * h);
        estimate += x;
    }

    estimate = estimate * h;
    return estimate;
}