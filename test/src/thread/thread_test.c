#include "thread_test.h"
#include "../test.h"
#include "../test_manager.h"

#include "../../../thread.h"

void* thread_func(void* arg) {
    int thread_id = *(int*)arg;
    printf("Thread %d: Starting\n", thread_id);

    // Simulate some work by sleeping
    thread_sleep(1000);

    printf("Thread %d: Ending\n", thread_id);
    return NULL;
}

#define NUM_THREADS    5
#define NUM_ITERATIONS 10

static int shared_counter = 0;
mutex counter_mutex;

void* mutex_thread_func(void* arg) {
    int thread_id = *(int*)arg;
    printf("Thread %d: Starting...\n", thread_id);

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        mutex_lock(&counter_mutex);
        int counter_value = shared_counter;
        thread_sleep(10);
        shared_counter = counter_value + 1;
        mutex_unlock(&counter_mutex);
    }

    printf("Thread %d: Ending...\n", thread_id);
    return NULL;
}

unsigned char thread_create_and_join_test() {
    thread threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Spawn multiple threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        u32 result = thread_create(&threads[i], thread_func, &thread_ids[i]);
        REQUIRE(result == 0);
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; ++i) {
        u32 result = thread_join(&threads[i]);
        REQUIRE(result == 0);
    }

    return true;
}

unsigned char thread_mutex_test() {
    thread threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    mutex_init(&counter_mutex);

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        u32 result = thread_create(&threads[i], mutex_thread_func, &thread_ids[i]);
        REQUIRE(result == 0);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        u32 result = thread_join(&threads[i]);
        REQUIRE(result == 0);
    }

    mutex_destroy(&counter_mutex);

    REQUIRE_TO_BE(NUM_THREADS * NUM_ITERATIONS, shared_counter);

    return true;
}

void thread_register_tests() {
    test_manager_register_test(thread_create_and_join_test, "Thread - Create and Join");
    test_manager_register_test(thread_mutex_test, "Thread - Mutex");
}