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
    double result = (function(a) + function(b)) / 2.0;

    # pragma omp parallel for num_threads (thread_count) reduction(+ : result)
    for (int i = 1; i <= n - 1 ; i++)
    {
        result += function(a + (i * h));
    }

    result = result * h;

    // # pragma omp parallel num_threads(thread_count) reduction(+:global_result)
    // global_result += 

    printf("Area of Trapezoid using %d threads is %f\n", thread_count, result);
    return 0;
}

double function(double x)
{
    return ((x*x*x) + (x*x) + x);
}


















/*#include <omp.h>
#include <stdio.h>

int main() {
    int x = 5;                                    // Line 1
    #pragma omp parallel num_threads(4) private(x)  // Line 2
    {                                             // Line 3
        int my_rank = omp_get_thread_num();       // Line 4
        printf("Thread %d > before initialization, x = %d\n", my_rank, x);  // Line 5
        x = 2 * my_rank + 2;                      // Line 6
        printf("Thread %d > after initialization, x = %d\n", my_rank, x);   // Line 7
    }                                             // Line 8
    printf("After parallel block, x = %d\n", x);  // Line 9
    return 0;                                     // Line 10
}*/