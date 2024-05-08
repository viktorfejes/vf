#include "test_manager.h"

#include <stdio.h>

typedef struct test_entry {
    PFN_test func;
    char* description;
} test_entry;

#define MAX_TESTS 100
static test_entry tests[MAX_TESTS];
static unsigned int test_count = 0;

void test_manager_init() {}

void test_manager_register_test(unsigned char (*PFN_test)(), char* desc) {
    test_entry entry = { .func = PFN_test, .description = desc };
    tests[test_count] = entry;
    test_count++;
}

void test_manager_run_tests() {
    unsigned int passed = 0;
    unsigned int failed = 0;
    unsigned int skipped = 0;

    for (unsigned int i = 0; i < test_count; ++i) {
        unsigned char result = tests[i].func();
        if (result == true) {
            ++passed;
        } else if (result == 2) {
            printf("[SKIPPED]: %s\n", tests[i].description);
            ++skipped;
        } else {
            printf("[FAILED]: %s\n", tests[i].description);
            ++failed;
        }
    }

    printf("Results: %d passed, %d failed, %d skipped\n", passed, failed, skipped);
}