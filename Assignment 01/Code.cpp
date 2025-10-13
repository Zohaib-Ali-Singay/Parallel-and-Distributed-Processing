#include <stdlib.h> 
#define N 19999 
 
int func1 ( int ) ; 
int func2 ( int ) ; 
 
int main ( ) 
{ 
    int i , j ; 
    j = 0 ; 
    for ( i = 0 ; i <= N; i++) 
    {  
        j = func1 ( j ) ; 
    } 
    return ( j ) ; 
} 
 
int func1 ( int j ) 
{ 
    int k ; 
    for ( k = 0 ; k <= N; k++) 
        j = func2 ( j ) ; 
    return ( j ) ; 
} 
 
int func2 ( int j ) 
{ 
    j += 1 - 3 / 4 ; 
    return j ; 
} 