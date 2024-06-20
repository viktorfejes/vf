/*
*   vf_test - v0.1
*   Header-only tiny test library for C/C++.
*
*   RECENT CHANGES:
*       0.1     (2024-06-20)    Finalized the implementation;
*
*   LICENSE: MIT License
*       Copyright (c) 2024 Viktor Fejes
*
*       Permission is hereby granted, free of charge, to any person obtaining a copy
*       of this software and associated documentation files (the "Software"), to deal
*       in the Software without restriction, including without limitation the rights
*       to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*       copies of the Software, and to permit persons to whom the Software is
*       furnished to do so, subject to the following conditions:
*
*       The above copyright notice and this permission notice shall be included in all
*       copies or substantial portions of the Software.
*
*       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*       IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*       FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*       AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*       LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*       OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*       SOFTWARE.
*
*   TODOs:
*       - [ ] 
*
 */

#ifndef VF_TEST_H
#define VF_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef bool (*PFN_test)(void);

// Test structure
typedef struct vf_test_case {
    const char* name;
    PFN_test func;
    struct vf_test_case* next;
} vf_test_case;

// Static global variables
static vf_test_case* vf_first_test = 0;
static vf_test_case* vf_last_test = 0;
static int max_name_length = 0;

// Main TEST
extern void _vf_register_test(bool (*PFN_test)(void), const char* name);
extern void _vf_run_all_tests(void);

// Macros...
#if defined(__GNUC__) || defined(__clang__)
    #define VF_TEST_REGISTER __attribute__((constructor))
    #define TEST(name)                                                          \
        bool test_func_##name(void);                                            \
        static void test_register_##name(void) VF_TEST_REGISTER;                \
        static void test_register_##name(void) {                                \
            _vf_register_test(test_func_##name, #name);                         \
        }                                                                       \
        bool test_func_##name(void)
#elif defined(_MSC_VER)
    #define VF_TEST_REGISTER_SECTION __pragma(section(".CRT$XCU", read))
    #define VF_TEST_REGISTER __declspec(allocate(".CRT$XCU"))
    #define TEST(name)                                                          \
        bool test_func_##name(void);                                            \
        static int test_register_##name(void) {                                 \
            _vf_register_test(test_func_##name, #name);                         \
            return 0;                                                           \
        }                                                                       \
        VF_TEST_REGISTER_SECTION VF_TEST_REGISTER int (*test_register_##name##_ptr)(void) = test_register_##name; \
        bool test_func_##name(void)
#else
    #error "Unsupported compiler"
#endif

// #define RUN_TEST()
#define RUN_ALL_TESTS() do {                    \
        printf("Test project " __FILE__ "\n");  \
        _vf_run_all_tests();                    \
    } while(0)

#define EXPECT_TRUE(condition) do {             \
        if(!(condition)) {                      \
            return false;                       \
        }                                       \
    } while (0)

#define EXPECT_FALSE(condition) EXPECT_TRUE(!(condition))

#define EXPECT_EQ(val1, val2) do { \
        if ((val1) != (val2)) { \
            return false; \
        } \
    } while (0)

#define EXPECT_NE(val1, val2) do { \
        if ((val1) == (val2)) { \
            return false; \
        } \
    } while (0)


#define FAIL(...)
#define SUCCEED(...)

#ifdef __cplusplus
}
#endif

// End of header...
#ifdef VF_TEST_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

// Register tests in a linked list...
void _vf_register_test(bool (*func)(void), const char* name) {
    vf_test_case* test = (vf_test_case*)malloc(sizeof(vf_test_case));
    test->name = name;
    test->func = func;

    int name_len = strlen(name);
    if (name_len > max_name_length) {
        max_name_length = name_len;
    }

    if (vf_last_test) {
        vf_last_test->next = test;
    } else {
        vf_first_test = test;
    }
    vf_last_test = test;
    test->next = 0;
}

static void _print_dots(const char* name) {
    int name_len = strlen(name);
    int diff = max_name_length - name_len;
    for (int i = 0; i < diff; ++i) {
        printf(".");
    }
}

void _vf_run_all_tests(void) {
    unsigned int passed = 0;
    unsigned int failed = 0;
    int count = 0;

    const char* ascii_format[] = {
        "\033[0m",      // reset
        "\033[96m",     // cyan
        "\033[92m",     // green
        "\033[93m",     // yellow
        "\033[91m",     // red
    };

    vf_test_case* test = vf_first_test;
    while (test) {
        printf("  Test #%d: ", count + 1);

        bool result = test->func();

        if (result == true) {
            printf("%s ", test->name);
            _print_dots(test->name);
            printf("... Passed\n");
            passed++;
        } else {
            printf("%s ", test->name);
            _print_dots(test->name);
            printf("... %sFailed%s\n", ascii_format[4], ascii_format[0]);
            // printf("FAILED: %s\n", test->name);
            failed++;
        }

        count++;
        test = test->next;
    }

    unsigned int percentage = passed / (float)count * 100.0f;
    if (failed <= 0) {
        printf("\nResults: %s%d%% tests passed%s, %d tests failed. (Total tests %d)\n", ascii_format[2], percentage, ascii_format[0], failed, count);
    } else {
        printf("\nResults: %d%% tests passed, %s%d tests failed%s. (Total tests %d)\n", percentage, ascii_format[4], failed, ascii_format[0], count);
    }

}

#endif // VF_TEST_IMPLEMENTATION
#endif // VF_TEST_H
