/* Custom RW Lock Structure */
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_ok;
    pthread_cond_t writers_ok;
    int readers;
    int writers;
    int waiting_writers;
} rwlock_t;

/* Initialize custom lock */
void rwlock_init(rwlock_t* lock) {
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_ok, NULL);
    pthread_cond_init(&lock->writers_ok, NULL);
    lock->readers = 0;
    lock->writers = 0;
    lock->waiting_writers = 0;
}


//Reader-Preference Acquire/Release

void rwlock_rdlock_reader_pref(rwlock_t* lock) {
    pthread_mutex_lock(&lock->mutex);
    while (lock->writers > 0 || lock->waiting_writers > 0)
        pthread_cond_wait(&lock->readers_ok, &lock->mutex);
    lock->readers++;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_wrlock_reader_pref(rwlock_t* lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->waiting_writers++;
    while (lock->writers > 0 || lock->readers > 0)
        pthread_cond_wait(&lock->writers_ok, &lock->mutex);
    lock->waiting_writers--;
    lock->writers++;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_unlock_reader_pref(rwlock_t* lock) {
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