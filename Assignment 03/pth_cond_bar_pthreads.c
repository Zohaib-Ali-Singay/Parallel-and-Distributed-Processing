/* File: pth_cond_bar_pthreads.c
 * Purpose: Use Pthreads barriers for synchronization.
 * Compile: gcc -g -Wall -o pth_cond_bar_pthreads pth_cond_bar_pthreads.c -lpthread
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define BARRIER_COUNT 100

int thread_count;
pthread_barrier_t barrier;  // Pthreads barrier

void Usage(char* prog_name);
void *Thread_work(void *rank);

int main(int argc, char* argv[]) {
   long thread;
   pthread_t* thread_handles; 
   double start, finish;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);

   thread_handles = malloc(thread_count * sizeof(pthread_t));
   pthread_barrier_init(&barrier, NULL, thread_count);  // Initialize barrier

   GET_TIME(start);
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL, Thread_work, (void*) thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);
   GET_TIME(finish);
   printf("Elapsed time = %e seconds\n", finish - start);

   pthread_barrier_destroy(&barrier);  // Cleanup
   free(thread_handles);
   return 0;
}

void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   exit(0);
}

void *Thread_work(void *rank) {
   int i;
   for (i = 0; i < BARRIER_COUNT; i++) {
      pthread_barrier_wait(&barrier);  // Use Pthreads barrier
   }
   return NULL;
}