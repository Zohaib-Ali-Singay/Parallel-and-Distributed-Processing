#include <stdio.h> 
#include <mpi.h> 
#include <string.h> 
 
double Trappozidal_Area(double start, double end, int no_traps, double step_size); 
double f(double x); 
 
int main(){ 
    double final_result, a=1, b=5 , local_a, local_b, h, local_integral; 
    int comm_sz, my_rank, n=1024, local_n, remaining, offset; 
 
    MPI_Init(NULL, NULL); 
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
 
    h = (b-a) / n; 
    local_n = n / comm_sz; 
    remaining = n % comm_sz; 
 
    //distributing extra intervals 
    for(int i=0; i < remaining; i++){ 
        if(my_rank == i){ 
            local_n++; 
        } 
    } 
     
    //offset calculation for Local_a 
    offset = (my_rank < remaining) ? my_rank : remaining;  
    local_a = a + (my_rank * (n/comm_sz) + offset) * h; 
 
    local_b = local_a + local_n * h;  
 
    local_integral = Trappozidal_Area(local_a, local_b, local_n, h); 
 
    if(my_rank != 0 ){ 
        MPI_Send(&local_integral, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD); 
    } 
    else{ 
        final_result = local_integral; 
for( int i=1; i < comm_sz; i++){ 
            MPI_Recv(&local_integral, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
            final_result += local_integral; 
        } 
 
        printf("With n =  %d trapezoids, our estimated of the integral Integral from %f to %f = %.15e \n", n, a, b, final_result); 
    } 
 
    MPI_Finalize(); 
    return 0; 
} 
 
double Trappozidal_Area(double start, double end, int no_traps, double 
step_size){ 
    double partial_result, x; 
    partial_result = (f(start)+f(end))/2.0; 
    for (int i=1; i<=no_traps-1; i++){ 
        x = start + i*step_size; 
        partial_result += f(x); 
    } 
    partial_result = partial_result*step_size; 
    return partial_result; 
} 
 
double f(double x){ 
    return 4 * x*x*x - 2 * x*x + 3 * x -5; 
} 