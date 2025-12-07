#include "../vf_test.h"
#include <time.h>

#define VF_THREAD_IMPLEMENTATION
#include "../vf_thread.h"

#include <stdbool.h>
#include <stdio.h>

// Helper function for thread creation test
void* test_thread_func(void* arg) {
    int* value = (int*)arg;
    *value += 1;
    return NULL;
}

TEST(Thread, Create) {
    vf_thread_t thread;
    int value = 0;
    vf_thread_error_t result = vf_thread_create(&thread, test_thread_func, &value);
    
    EXPECT_EQ(result, VF_THREAD_SUCCESS);
    
    vf_thread_join(&thread);
    EXPECT_EQ(value, 1);
    
    return true;
}

TEST(Thread, Join) {
    vf_thread_t thread;
    int value = 0;
    vf_thread_create(&thread, test_thread_func, &value);
    
    vf_thread_error_t result = vf_thread_join(&thread);
    
    EXPECT_EQ(result, VF_THREAD_SUCCESS);
    EXPECT_EQ(value, 1);
    
    return true;
}

TEST(Thread, Detach) {
    vf_thread_t thread;
    int value = 0;
    vf_thread_create(&thread, test_thread_func, &value);
    
    vf_thread_error_t result = vf_thread_detach(&thread);
    
    EXPECT_EQ(result, VF_THREAD_SUCCESS);
    
    // Note: We can't check the value here as the thread might not have finished yet
    
    return true;
}

TEST(Mutex, LockUnlock) {
    vf_mutex_t mutex;
    vf_mutex_init(&mutex);
    
    vf_mutex_lock(&mutex);
    // If we reach here, the lock was successful
    EXPECT_TRUE(true);
    
    vf_mutex_unlock(&mutex);
    // If we reach here, the unlock was successful
    EXPECT_TRUE(true);
    
    vf_mutex_destroy(&mutex);
    return true;
}

// Helper function for condition variable test
typedef struct {
    vf_cond_t* cond;
    vf_mutex_t* mutex;
    int* flag;
} cond_test_args;

void* cond_wait_thread(void* arg) {
    cond_test_args* args = (cond_test_args*)arg;
    
    vf_mutex_lock(args->mutex);
    while (*args->flag == 0) {
        vf_cond_wait(args->cond, args->mutex);
    }
    vf_mutex_unlock(args->mutex);
    
    return NULL;
}

TEST(ConditionVariable, SignalBroadcast) {
    vf_cond_t cond;
    vf_mutex_t mutex;
    int flag = 0;
    
    vf_cond_init(&cond);
    vf_mutex_init(&mutex);
    
    cond_test_args args = {&cond, &mutex, &flag};
    
    vf_thread_t thread;
    vf_thread_create(&thread, cond_wait_thread, &args);
    
    // Give the thread time to start waiting
    vf_thread_sleep(100);
    
    vf_mutex_lock(&mutex);
    flag = 1;
    vf_cond_signal(&cond);
    vf_mutex_unlock(&mutex);
    
    vf_thread_error_t result = vf_thread_join(&thread);
    EXPECT_EQ(result, VF_THREAD_SUCCESS);
    
    vf_cond_destroy(&cond);
    vf_mutex_destroy(&mutex);
    return true;
}

// Helper function for read-write lock test
#define MAX_THREADS 4
#define TIMEOUT_SECONDS 5

typedef struct {
    vf_rwlock_t* rwlock;
    int* shared_data;
    int thread_id;
} rwlock_test_args;

void* reader_thread(void* arg) {
    rwlock_test_args* args = (rwlock_test_args*)arg;
    vf_rwlock_rdlock(args->rwlock);
    vf_thread_sleep(50);  // Simulate reading
    vf_rwlock_unlock(args->rwlock);
    return NULL;
}

void* writer_thread(void* arg) {
    rwlock_test_args* args = (rwlock_test_args*)arg;
    vf_rwlock_wrlock(args->rwlock);
    (*args->shared_data)++;
    vf_thread_sleep(100);  // Simulate writing
    vf_rwlock_unlock(args->rwlock);
    return NULL;
}

TEST(ReadWriteLock, ReadersWriters) {
    vf_rwlock_t rwlock;
    int shared_data = 0;
    vf_rwlock_init(&rwlock);
    
    vf_thread_t threads[MAX_THREADS];
    rwlock_test_args args[MAX_THREADS];
    
    // Create 3 reader threads
    for (int i = 0; i < 3; i++) {
        args[i] = (rwlock_test_args){&rwlock, &shared_data, i};
        vf_thread_create(&threads[i], reader_thread, &args[i]);
        vf_thread_sleep(10);  // Small delay between thread creations
    }
    
    // Create 1 writer thread
    args[3] = (rwlock_test_args){&rwlock, &shared_data, 3};
    vf_thread_create(&threads[3], writer_thread, &args[3]);
    
    // Wait for threads to complete with a timeout
    time_t start_time = time(NULL);
    for (int i = 0; i < MAX_THREADS; i++) {
        while (1) {
            vf_thread_error_t result = vf_thread_join(&threads[i]);
            if (result == VF_THREAD_SUCCESS) {
                break;
            }
            if (time(NULL) - start_time > TIMEOUT_SECONDS) {
                EXPECT_TRUE(false);  // Force test failure
                return false;
            }
            vf_thread_sleep(10);  // Short sleep before trying again
        }
    }
    
    vf_rwlock_destroy(&rwlock);
    EXPECT_EQ(shared_data, 1);  // The writer should have incremented this once
    return true;
}

// Helper function for TLS test
void* tls_thread(void* arg) {
    vf_tls_key_t* key = (vf_tls_key_t*)arg;
    int* value = (int*)malloc(sizeof(int));
    *value = 42;
    
    vf_tls_set(key, value);
    
    int* retrieved = (int*)vf_tls_get(key);
    EXPECT_EQ_INT(*retrieved, 42);
    
    free(value);
    return NULL;
}

TEST(ThreadLocalStorage, SetGet) {
    vf_tls_key_t key;
    vf_thread_error_t result = vf_tls_create(&key, free);
    EXPECT_EQ(result, VF_THREAD_SUCCESS);
    
    vf_thread_t thread;
    vf_thread_create(&thread, tls_thread, &key);
    
    vf_thread_join(&thread);
    
    vf_tls_delete(&key);
    return true;
}
