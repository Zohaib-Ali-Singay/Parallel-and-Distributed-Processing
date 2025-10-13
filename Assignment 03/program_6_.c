#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"

const int MAX_KEY = 100000000;

struct list_node_s {
   int data;
   struct list_node_s* next;
};

struct list_node_s* head = NULL;  
int thread_count;
int total_ops;
double insert_percent;
double search_percent;
double delete_percent;
pthread_mutex_t count_mutex;
int member_count = 0, insert_count = 0, delete_count = 0;
int writer_preference = 0;  // 0 = reader preference, 1 = writer preference

/* Custom RW lock */
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_ok;
    pthread_cond_t writers_ok;
    int readers;
    int writers;
    int waiting_writers;
} rwlock_t;

rwlock_t rwlock;

void rwlock_init(rwlock_t* lock) {
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_ok, NULL);
    pthread_cond_init(&lock->writers_ok, NULL);
    lock->readers = 0;
    lock->writers = 0;
    lock->waiting_writers = 0;
}

void rwlock_rdlock(rwlock_t* lock) {
    pthread_mutex_lock(&lock->mutex);
    if (writer_preference) {
        while (lock->writers > 0 || lock->waiting_writers > 0)
            pthread_cond_wait(&lock->readers_ok, &lock->mutex);
    } else {
        while (lock->writers > 0)
            pthread_cond_wait(&lock->readers_ok, &lock->mutex);
    }
    lock->readers++;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_wrlock(rwlock_t* lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->waiting_writers++;
    while (lock->writers > 0 || lock->readers > 0)
        pthread_cond_wait(&lock->writers_ok, &lock->mutex);
    lock->waiting_writers--;
    lock->writers++;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_unlock(rwlock_t* lock) {
    pthread_mutex_lock(&lock->mutex);
    if (lock->writers > 0) {
        lock->writers = 0;
    } else {
        lock->readers--;
    }
    if (lock->waiting_writers > 0 && lock->readers == 0) {
        pthread_cond_signal(&lock->writers_ok);
    } else {
        pthread_cond_broadcast(&lock->readers_ok);
    }
    pthread_mutex_unlock(&lock->mutex);
}

void Usage(char* prog_name);
void Get_input(int* inserts_in_main_p);
void* Thread_work(void* rank);
int Insert(int value);
int Member(int value);
int Delete(int value);
void Free_list(void);
int Is_empty(void);

int main(int argc, char* argv[]) {
   long i; 
   int key, success, attempts;
   pthread_t* thread_handles;
   int inserts_in_main;
   unsigned seed = 1;
   double start, finish;

   if (argc < 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   if (argc == 3 && atoi(argv[2]) == 1) writer_preference = 1;

   Get_input(&inserts_in_main);

   i = attempts = 0;
   while (i < inserts_in_main && attempts < 2*inserts_in_main) {
      key = my_rand(&seed) % MAX_KEY;
      success = Insert(key);
      attempts++;
      if (success) i++;
   }
   printf("Inserted %ld keys in empty list\n", i);
   printf("Using %s preference mode\n", writer_preference ? "Writer" : "Reader");

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   pthread_mutex_init(&count_mutex, NULL);
   rwlock_init(&rwlock);

   GET_TIME(start);
   for (i = 0; i < thread_count; i++)
      pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);

   for (i = 0; i < thread_count; i++)
      pthread_join(thread_handles[i], NULL);
   GET_TIME(finish);

   printf("Elapsed time = %e seconds\n", finish - start);
   printf("Total ops = %d\n", total_ops);
   printf("member ops = %d\n", member_count);
   printf("insert ops = %d\n", insert_count);
   printf("delete ops = %d\n", delete_count);

   Free_list();
   pthread_mutex_destroy(&count_mutex);
   free(thread_handles);

   return 0;
}

void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count> [writer_preference: 0|1]\n", prog_name);
   exit(0);
}

void Get_input(int* inserts_in_main_p) {
   printf("How many keys should be inserted in the main thread?\n");
   scanf("%d", inserts_in_main_p);
   printf("How many ops total should be executed?\n");
   scanf("%d", &total_ops);
   printf("Percent of ops that should be searches? (between 0 and 1)\n");
   scanf("%lf", &search_percent);
   printf("Percent of ops that should be inserts? (between 0 and 1)\n");
   scanf("%lf", &insert_percent);
   delete_percent = 1.0 - (search_percent + insert_percent);
}

int Insert(int value) {
   struct list_node_s *curr = head, *pred = NULL, *temp;
   int rv = 1;
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      temp->data = value;
      temp->next = curr;
      if (pred == NULL)
         head = temp;
      else
         pred->next = temp;
   } else {
      rv = 0;
   }
   return rv;
}

int Member(int value) {
   struct list_node_s* temp = head;
   while (temp != NULL && temp->data < value)
      temp = temp->next;
   return (temp != NULL && temp->data == value);
}

int Delete(int value) {
   struct list_node_s *curr = head, *pred = NULL;
   int rv = 1;
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) head = curr->next;
      else pred->next = curr->next;
      free(curr);
   } else {
      rv = 0;
   }
   return rv;
}

void Free_list(void) {
   struct list_node_s *current = head, *following;
   while (current != NULL) {
      following = current->next;
      free(current);
      current = following;
   }
}

int Is_empty(void) {
   return head == NULL;
}

void* Thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, val;
   double which_op;
   unsigned seed = my_rank + 1;
   int my_member_count = 0, my_insert_count = 0, my_delete_count = 0;
   int ops_per_thread = total_ops/thread_count;

   for (i = 0; i < ops_per_thread; i++) {
      which_op = my_drand(&seed);
      val = my_rand(&seed) % MAX_KEY;
      if (which_op < search_percent) {
         rwlock_rdlock(&rwlock);
         Member(val);
         rwlock_unlock(&rwlock);
         my_member_count++;
      } else if (which_op < search_percent + insert_percent) {
         rwlock_wrlock(&rwlock);
         Insert(val);
         rwlock_unlock(&rwlock);
         my_insert_count++;
      } else {
         rwlock_wrlock(&rwlock);
         Delete(val);
         rwlock_unlock(&rwlock);
         my_delete_count++;
      }
   }

   pthread_mutex_lock(&count_mutex);
   member_count += my_member_count;
   insert_count += my_insert_count;
   delete_count += my_delete_count;
   pthread_mutex_unlock(&count_mutex);

    return NULL;
}