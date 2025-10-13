#include<stdio.h>
#include<mpi.h>
#include<math.h>
double Trapezoidal_Area(double local_a, double local_b, int local_n, double h);
double function(double a);
int main()
{
    double a = 0.0, b = 4.0, local_int, total_int, h, local_a, local_b; 
    int local_n, comm_size, my_rank, n = 24000;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    h = (b-a)/n;
    local_n = n / comm_size;
    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;

    local_int = Trapezoidal_Area(local_a, local_b, local_n, h);

    if (my_rank != 0)
    {
        MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    else
    {
        total_int = local_int;
        for (int q = 1; q <= comm_size - 1; q++)
        {
            MPI_Recv(&local_int, 1, MPI_DOUBLE, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_int += local_int;
        }

        printf("With n = %d trapezoids, our estimate \n", n);
        printf("of the integral from %f to %f = %.15e\n", a, b, total_int);
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