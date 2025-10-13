#include<stdio.h>
# ifdef _OPENMP
    #include<omp.h>
# endif
#include<stdlib.h>

double Trapezoid(int a, int b, int n, double h);
double function(double x);

int main(int argc, char* argv[])
{
    int a=10, b=30, n=1000, thread_count = strtol(argv[1], NULL, 10);
    double h = (double)(b - a) / n;
    double global_result=0.0;
    
    # pragma omp parallel num_threads(thread_count)
    {
        double my_result = 0.0;
        my_result += Trapezoid(a, b, n, h);
        # pragma omp critical
        global_result += my_result;
    }

    // # pragma omp parallel num_threads(thread_count) reduction(+:global_result)
    // global_result += 

    printf("Area of Trapezoid using %d threads is %f\n", thread_count, global_result);
    return 0;
}

double Trapezoid(int a, int b, int n, double h)
{
    int thread_count = omp_get_num_threads();
    int my_rank = omp_get_thread_num();
    int local_n = n / thread_count;
    
    double local_a = a + (my_rank * local_n * h);
    double local_b = local_a + (local_n * h);
    double x;
    double my_result = (function(local_a) + function(local_b)) / 2.0;

    for (int i = 1; i <= local_n - 1 ; i++)
    {
        x = local_a + (i * h);
        my_result += function(x);
    }

    my_result = my_result * h;
    return my_result;
}

double function(double x)
{
    return ((x*x*x) + (x*x) + x);
}