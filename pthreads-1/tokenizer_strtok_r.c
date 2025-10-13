#include<semaphore.h>
#include<pthread.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

long thread_count;
void *Tokenizer(void *rank);
sem_t *sem; 

int main(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
    sem = malloc(thread_count * sizeof(sem_t));

    /*
        Scenario: All sems Elements Initialized to 1
Thread Execution:
Each thread executes sem_wait(&sems[my_rank]).
If sems[my_rank] is 1 for every thread’s rank, sem_wait succeeds immediately (decrementing the semaphore to 0) without blocking, because the initial value allows the first call to pass.
All threads can then proceed to fg_rv = fgets(my_line, MAX, stdin) at the same time.
Behavior with stdin:
stdin (standard input) is a shared resource, typically a single input stream (e.g., from the keyboard or a redirected file).
If multiple threads call fgets on stdin simultaneously, the behavior is undefined in standard C. Possible outcomes include:
Interleaved Input: Lines of input might be split unpredictably between threads.
Race Condition: One thread might read a line entirely, leaving others with nothing or errors.
Corruption: Concurrent access could garble the input data.
In practice, stdin is not thread-safe for concurrent reads without additional synchronization (e.g., a separate mutex), and the POSIX standard doesn’t guarantee safe concurrent access.
After Input:
Each thread would then call sem_post(&sems[next]), signaling the next thread. However, since all threads already proceeded past sem_wait, this signal might not coordinate further execution as intended unless the loop repeats with another sem_wait.
Your Observation’s Correctness
Technically: Yes, if all sems elements are initialized to 1, all threads could initially pass sem_wait and attempt to read from stdin simultaneously. So, your statement that “they will take input from the user” is true in the sense that they can all reach the fgets call.
Practically: This would lead to problematic behavior (e.g., race conditions or corrupted input), which contradicts the code’s apparent purpose of serializing access to input lines for tokenization in a round-robin manner. The design with sem_post(&sems[next]) implies that only one thread should read at a time, passing control to the next thread sequentially.
    */

    sem_init(&sem[0], 0, 1);

    for(long thread = 1; thread < thread_count; thread++)
        sem_init(&sem[thread], 0, 0);

    for(long thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, Tokenizer, (void *) thread);
    }

    for(long thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }

    for(long thread = 0; thread < thread_count; thread++)
    {
        sem_destroy(&sem[thread]);
    }

    printf("Program successfully terminated\n");

    return 0;
}

void *Tokenizer(void *rank)
{
    long my_rank = (long) rank;

    long next = (my_rank + 1) % thread_count;
    int count;
    char *fg_rv, my_line[100], *my_string;
    char *saveptr;

    sem_wait(&sem[my_rank]);
    fg_rv = fgets(my_line, 100, stdin);
    sem_post(&sem[next]);

    while(fg_rv != NULL)
    {
        printf("Thread %ld > my_line = %s\n", my_rank, my_line);

        count = 0;
        my_string = strtok_r(my_line, " \t\n", &saveptr);

        while(my_string != NULL)
        {
            count++;
            printf("Thread %ld > string %d = %s\n", my_rank, count, my_string);
            my_string = strtok_r(NULL, " \t\n", &saveptr);
        }

        sem_wait(&sem[my_rank]);
        fg_rv = fgets(my_line, 100, stdin);
        sem_post(&sem[next]);
    }

    return NULL;
}