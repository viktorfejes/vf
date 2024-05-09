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

unsigned char thread_create_and_join_test() {
    const int num_threads = 5;
    thread threads[num_threads];
    int thread_ids[num_threads];

    // Spawn multiple threads
    for (int i = 0; i < num_threads; ++i) {
        thread_ids[i] = i;
        u32 result = thread_create(&threads[i], thread_func, &thread_ids[i]);
        REQUIRE(result == 0);
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_threads; ++i) {
        u32 result = thread_join(&threads[i]);
        REQUIRE(result == 0);
    }

    return true;
}

void thread_register_tests() {
    test_manager_register_test(thread_create_and_join_test, "Thread - Create and Join");
}