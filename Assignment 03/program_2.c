#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define NUM_THREADS 4
long long int number_of_tosses;
long long int number_in_circle = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* monte_carlo(void* arg) {
    long long local_count = 0;
    unsigned int seed = time(NULL) ^ pthread_self(); // Unique seed per thread

    for (long long i = 0; i < number_of_tosses / NUM_THREADS; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0; // [-1, 1]
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0; // [-1, 1]
        if (x*x + y*y <= 1.0) local_count++;
    }

    pthread_mutex_lock(&mutex);
    number_in_circle += local_count;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char* argv[]) {

    number_of_tosses = atoll(argv[1]);
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, monte_carlo, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double pi_estimate = 4.0 * number_in_circle / number_of_tosses;
    printf("Estimated π = %.15f\n", pi_estimate);
    printf("Actual π    = %.15f\n", M_PI);
    printf("Difference  = %.15f\n", fabs(M_PI - pi_estimate));

    return 0;
}