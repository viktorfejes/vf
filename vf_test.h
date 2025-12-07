/*
 *   vf_test - v1.0
 *   Header-only tiny test library for C/C++.
 *
 *   RECENT CHANGES:
 *       1.0     (2025-12-06)    Complete rewrite of the test library.
 *       0.22    (2024-08-04)    Added Greater Than or Equal To macro;
 *                               Added EXPECT_EQ_INT for integer equality;
 *       0.21    (2024-06-24)    Added Greater Than macro;
 *       0.2     (2024-06-21)    Added suite to TEST;
 *       0.1     (2024-06-20)    Finalized the implementation;
 *
 *   LICENSE: MIT License
 *       Copyright (c) 2025 Viktor Fejes
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
 *
 */

#ifndef VF_TEST_H
#define VF_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------
   Configuration flags (user-overridable)
   ------------------------------------------ */

/* Enable/disable ANSI colour output (enabled by default) */
#ifndef VF_TEST_ENABLE_COLOR
#    define VF_TEST_ENABLE_COLOR 1
#endif

/* Default column alignment for test names */
#ifndef VF_TEST_NAME_ALIGN
#    define VF_TEST_NAME_ALIGN 28
#endif

/* Set max number of tests (for static allocation) */
#ifndef VF_TEST_MAX_TESTS
#    define VF_TEST_MAX_TESTS 128
#endif

/* Enable/disable auto-register tests */
#ifndef VF_TEST_ENABLE_AUTO_REGISTER
#    define VF_TEST_ENABLE_AUTO_REGISTER 1
#endif

/* Enable/disable test sorting */
#ifndef VF_TEST_ENABLE_SORTING
#    define VF_TEST_ENABLE_SORTING 1
#endif

#include <stdint.h>

/* ------------------------------------------
   Auxilliary bits
   ------------------------------------------ */

/* Avoid including full string.h */
static int vf_test_strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char *)a - *(const unsigned char *)b;
}

static int vf_test_memcmp(const void *a, const void *b, unsigned long long n) {
    const unsigned char *pa = (const unsigned char *)a;
    const unsigned char *pb = (const unsigned char *)b;

    for (unsigned long long i = 0; i < n; i++) {
        unsigned char da = pa[i];
        unsigned char db = pb[i];
        if (da != db) return (int)da - (int)db;
    }
    return 0;
}

/* Avoid windows.h include */
#if defined(_WIN32)
typedef struct {
    long long QuadPart;
} vf_test_large_integer;

__declspec(dllimport) int __stdcall QueryPerformanceCounter(vf_test_large_integer *lpCounter);
__declspec(dllimport) int __stdcall QueryPerformanceFrequency(vf_test_large_integer *lpFreq);
#endif

/* Platform detection for auto-register constructor */
#if defined(__GNUC__) || defined(__clang__)
#    define VF_TEST_CONSTRUCTOR __attribute__((constructor))
#elif defined(_MSC_VER)
#    define VF_TEST_CONSTRUCTOR_SECTION __pragma(section(".CRT$XCU", read))
#    define VF_TEST_CONSTRUCTOR         __declspec(allocate(".CRT$XCU"))
#else
#    error "Unsupported compiler for auto test registration"
#endif

/* ------------------------------------------
   Public Types
   ------------------------------------------ */

typedef enum {
    VF_TEST_RESULT_NONE = 0,
    VF_TEST_RESULT_PASS = 1,
    VF_TEST_RESULT_FAIL = 2,
    VF_TEST_RESULT_SKIP = 3,
} VFTestResult;

typedef void (*PFN_vf_test)(void);

typedef struct {
    const char  *suite;
    const char  *name;
    VFTestResult result;
    uint64_t     time_ns;
    PFN_vf_test  pfn_fn;
} VFTestCase;

extern void vf_test_register_one(
    const char *suite,
    const char *name,
    PFN_vf_test pfn_fn);

extern void vf_test_record_skip(
    const char *file,
    int         line,
    const char *reason);

void vf_test_record_failure(
    const char *file,
    int         line,
    const char *msg_fmt,
    ...);

/* ------------------------------------------
   ASSERT/EXPECT Macros
   ------------------------------------------ */

/* EQUALITY checks */

#define VF_ASSERT_EQ_INT(exp, act)                                         \
    do {                                                                   \
        int _exp_val = (exp);                                              \
        int _act_val = (act);                                              \
        if (_exp_val != _act_val) {                                        \
            vf_test_record_failure(__FILE__, __LINE__,                     \
                                   "Expected %d, got %d (expr: %s == %s)", \
                                   _exp_val, _act_val, #exp, #act);        \
            return;                                                        \
        }                                                                  \
    } while (0)

#define VF_EXPECT_EQ_INT(exp, act)                                         \
    do {                                                                   \
        int _exp_val = (exp);                                              \
        int _act_val = (act);                                              \
        if (_exp_val != _act_val) {                                        \
            vf_test_record_failure(__FILE__, __LINE__,                     \
                                   "Expected %d, got %d (expr: %s == %s)", \
                                   _exp_val, _act_val, #exp, #act);        \
        }                                                                  \
    } while (0)

#define VF_EXPECT_NE_INT(exp, act)                                        \
    do {                                                                  \
        int _exp_val = (exp);                                             \
        int _act_val = (act);                                             \
        if (_exp_val == _act_val) {                                       \
            vf_test_record_failure(__FILE__, __LINE__,                    \
                                   "Expected %s != %s, but both were %d", \
                                   #exp, #act, _act_val);                 \
        }                                                                 \
    } while (0)

#define VF_ASSERT_NE_INT(exp, act)                                        \
    do {                                                                  \
        int _exp_val = (exp);                                             \
        int _act_val = (act);                                             \
        if (_exp_val == _act_val) {                                       \
            vf_test_record_failure(__FILE__, __LINE__,                    \
                                   "Expected %s != %s, but both were %d", \
                                   #exp, #act, _act_val);                 \
            return;                                                       \
        }                                                                 \
    } while (0)

#define VF_EXPECT_NE_UINT(exp, act)                                       \
    do {                                                                  \
        unsigned _exp_val = (exp);                                        \
        unsigned _act_val = (act);                                        \
        if (_exp_val == _act_val) {                                       \
            vf_test_record_failure(__FILE__, __LINE__,                    \
                                   "Expected %s != %s, but both were %u", \
                                   #exp, #act, _act_val);                 \
        }                                                                 \
    } while (0)

#define VF_ASSERT_NE_UINT(exp, act)                                       \
    do {                                                                  \
        unsigned _exp_val = (exp);                                        \
        unsigned _act_val = (act);                                        \
        if (_exp_val == _act_val) {                                       \
            vf_test_record_failure(__FILE__, __LINE__,                    \
                                   "Expected %s != %s, but both were %u", \
                                   #exp, #act, _act_val);                 \
            return;                                                       \
        }                                                                 \
    } while (0)

#define VF_EXPECT_EQ_FLOAT(exp, act)                                                     \
    do {                                                                                 \
        float _exp_val = (exp);                                                          \
        float _act_val = (act);                                                          \
        if (_exp_val != _act_val) {                                                      \
            vf_test_record_failure(__FILE__, __LINE__,                                   \
                                   "Expected %s == %s (%.6f == %.6f) but they differed", \
                                   #exp, #act, _exp_val, _act_val);                      \
        }                                                                                \
    } while (0)

#define VF_ASSERT_EQ_FLOAT(exp, act)                                                     \
    do {                                                                                 \
        float _exp_val = (exp);                                                          \
        float _act_val = (act);                                                          \
        if (_exp_val != _act_val) {                                                      \
            vf_test_record_failure(__FILE__, __LINE__,                                   \
                                   "Expected %s == %s (%.6f == %.6f) but they differed", \
                                   #exp, #act, _exp_val, _act_val);                      \
            return;                                                                      \
        }                                                                                \
    } while (0)

#define VF_EXPECT_NE_FLOAT(exp, act)                                       \
    do {                                                                   \
        float _exp_val = (exp);                                            \
        float _act_val = (act);                                            \
        if (_exp_val == _act_val) {                                        \
            vf_test_record_failure(__FILE__, __LINE__,                     \
                                   "Expected %s != %s but both were %.6f", \
                                   #exp, #act, _act_val);                  \
        }                                                                  \
    } while (0)

#define VF_ASSERT_NE_FLOAT(exp, act)                                       \
    do {                                                                   \
        float _exp_val = (exp);                                            \
        float _act_val = (act);                                            \
        if (_exp_val == _act_val) {                                        \
            vf_test_record_failure(__FILE__, __LINE__,                     \
                                   "Expected %s != %s but both were %.6f", \
                                   #exp, #act, _act_val);                  \
            return;                                                        \
        }                                                                  \
    } while (0)

#define VF_EXPECT_NEAR_FLOAT(exp, act, eps)                                         \
    do {                                                                            \
        float _exp_val = (exp);                                                     \
        float _act_val = (act);                                                     \
        float _eps_val = (eps);                                                     \
        float _diff = _act_val - _exp_val;                                          \
        if (_diff < 0) _diff = -_diff;                                              \
        if (_diff > _eps_val) {                                                     \
            vf_test_record_failure(__FILE__, __LINE__,                              \
                                   "Expected %s ~= %s (%.6f +- %.6f) but got %.6f", \
                                   #exp, #act, _exp_val, _eps_val, _act_val);       \
        }                                                                           \
    } while (0)

#define VF_ASSERT_NEAR_FLOAT(exp, act, eps)                                         \
    do {                                                                            \
        float _exp_val = (exp);                                                     \
        float _act_val = (act);                                                     \
        float _eps_val = (eps);                                                     \
        float _diff = _act_val - _exp_val;                                          \
        if (_diff < 0) _diff = -_diff;                                              \
        if (_diff > _eps_val) {                                                     \
            vf_test_record_failure(__FILE__, __LINE__,                              \
                                   "Expected %s ~= %s (%.6f +- %.6f) but got %.6f", \
                                   #exp, #act, _exp_val, _eps_val, _act_val);       \
            return;                                                                 \
        }                                                                           \
    } while (0)

#define VF_EXPECT_EQ_STR(exp, act)                                                 \
    do {                                                                           \
        const char *_exp_val = (exp);                                              \
        const char *_act_val = (act);                                              \
        if (!_exp_val || !_act_val || vf_test_strcmp(_exp_val, _act_val) != 0) {   \
            vf_test_record_failure(__FILE__, __LINE__,                             \
                                   "Expected \"%s\", got \"%s\" (expr: %s == %s)", \
                                   _exp_val ? _exp_val : "(null)",                 \
                                   _act_val ? _act_val : "(null)",                 \
                                   #exp, #act);                                    \
        }                                                                          \
    } while (0)

#define VF_ASSERT_EQ_STR(exp, act)                                                 \
    do {                                                                           \
        const char *_exp_val = (exp);                                              \
        const char *_act_val = (act);                                              \
        if (!_exp_val || !_act_val || vf_test_strcmp(_exp_val, _act_val) != 0) {   \
            vf_test_record_failure(__FILE__, __LINE__,                             \
                                   "Expected \"%s\", got \"%s\" (expr: %s == %s)", \
                                   _exp_val ? _exp_val : "(null)",                 \
                                   _act_val ? _act_val : "(null)",                 \
                                   #exp, #act);                                    \
            return;                                                                \
        }                                                                          \
    } while (0)

/* INEQUALITIES checks */

#define VF_EXPECT_GT(a, b)                                                        \
    do {                                                                          \
        double _av = (double)(a);                                                 \
        double _bv = (double)(b);                                                 \
        if (!(_av > _bv)) {                                                       \
            vf_test_record_failure(__FILE__, __LINE__,                            \
                                   "Expected %s > %s, but %.6f > %.6f was false", \
                                   #a, #b, _av, _bv);                             \
        }                                                                         \
    } while (0)

#define VF_ASSERT_GT(a, b)                                                        \
    do {                                                                          \
        double _av = (double)(a);                                                 \
        double _bv = (double)(b);                                                 \
        if (!(_av > _bv)) {                                                       \
            vf_test_record_failure(__FILE__, __LINE__,                            \
                                   "Expected %s > %s, but %.6f > %.6f was false", \
                                   #a, #b, _av, _bv);                             \
            return;                                                               \
        }                                                                         \
    } while (0)

#define VF_EXPECT_GE(a, b)                                                          \
    do {                                                                            \
        double _av = (double)(a);                                                   \
        double _bv = (double)(b);                                                   \
        if (!(_av >= _bv)) {                                                        \
            vf_test_record_failure(__FILE__, __LINE__,                              \
                                   "Expected %s >= %s, but %.6f >= %.6f was false", \
                                   #a, #b, _av, _bv);                               \
        }                                                                           \
    } while (0)

#define VF_ASSERT_GE(a, b)                                                          \
    do {                                                                            \
        double _av = (double)(a);                                                   \
        double _bv = (double)(b);                                                   \
        if (!(_av >= _bv)) {                                                        \
            vf_test_record_failure(__FILE__, __LINE__,                              \
                                   "Expected %s >= %s, but %.6f >= %.6f was false", \
                                   #a, #b, _av, _bv);                               \
            return;                                                                 \
        }                                                                           \
    } while (0)

#define VF_EXPECT_LT(a, b)                                                        \
    do {                                                                          \
        double _av = (double)(a);                                                 \
        double _bv = (double)(b);                                                 \
        if (!(_av < _bv)) {                                                       \
            vf_test_record_failure(__FILE__, __LINE__,                            \
                                   "Expected %s < %s, but %.6f < %.6f was false", \
                                   #a, #b, _av, _bv);                             \
        }                                                                         \
    } while (0)

#define VF_ASSERT_LT(a, b)                                                        \
    do {                                                                          \
        double _av = (double)(a);                                                 \
        double _bv = (double)(b);                                                 \
        if (!(_av < _bv)) {                                                       \
            vf_test_record_failure(__FILE__, __LINE__,                            \
                                   "Expected %s < %s, but %.6f < %.6f was false", \
                                   #a, #b, _av, _bv);                             \
            return;                                                               \
        }                                                                         \
    } while (0)

#define VF_EXPECT_LE(a, b)                                                          \
    do {                                                                            \
        double _av = (double)(a);                                                   \
        double _bv = (double)(b);                                                   \
        if (!(_av <= _bv)) {                                                        \
            vf_test_record_failure(__FILE__, __LINE__,                              \
                                   "Expected %s <= %s, but %.6f <= %.6f was false", \
                                   #a, #b, _av, _bv);                               \
        }                                                                           \
    } while (0)

#define VF_ASSERT_LE(a, b)                                                          \
    do {                                                                            \
        double _av = (double)(a);                                                   \
        double _bv = (double)(b);                                                   \
        if (!(_av <= _bv)) {                                                        \
            vf_test_record_failure(__FILE__, __LINE__,                              \
                                   "Expected %s <= %s, but %.6f <= %.6f was false", \
                                   #a, #b, _av, _bv);                               \
            return;                                                                 \
        }                                                                           \
    } while (0)

/* NULL checks */

#define VF_EXPECT_NULL(ptr)                                              \
    do {                                                                 \
        const void *_p = (ptr);                                          \
        if (_p != NULL) {                                                \
            vf_test_record_failure(__FILE__, __LINE__,                   \
                                   "Expected %s to be NULL, but was %p", \
                                   #ptr, _p);                            \
        }                                                                \
    } while (0)

#define VF_ASSERT_NULL(ptr)                                              \
    do {                                                                 \
        const void *_p = (ptr);                                          \
        if (_p != NULL) {                                                \
            vf_test_record_failure(__FILE__, __LINE__,                   \
                                   "Expected %s to be NULL, but was %p", \
                                   #ptr, _p);                            \
            return;                                                      \
        }                                                                \
    } while (0)

#define VF_EXPECT_NOT_NULL(ptr)                                                \
    do {                                                                       \
        const void *_p = (ptr);                                                \
        if (_p == NULL) {                                                      \
            vf_test_record_failure(__FILE__, __LINE__,                         \
                                   "Expected %s to be non-NULL, but was NULL", \
                                   #ptr);                                      \
        }                                                                      \
    } while (0)

#define VF_ASSERT_NOT_NULL(ptr)                                                \
    do {                                                                       \
        const void *_p = (ptr);                                                \
        if (_p == NULL) {                                                      \
            vf_test_record_failure(__FILE__, __LINE__,                         \
                                   "Expected %s to be non-NULL, but was NULL", \
                                   #ptr);                                      \
            return;                                                            \
        }                                                                      \
    } while (0)

/* Pointer Equality checks */
#define VF_EXPECT_EQ_PTR(exp, act)                                                   \
    do {                                                                             \
        const void *_exp_val = (const void *)(exp);                                  \
        const void *_act_val = (const void *)(act);                                  \
        if (_exp_val != _act_val) {                                                  \
            vf_test_record_failure(__FILE__, __LINE__,                               \
                                   "Expected %s == %s (%p == %p) but they differed", \
                                   #exp, #act, _exp_val, _act_val);                  \
        }                                                                            \
    } while (0)

#define VF_ASSERT_EQ_PTR(exp, act)                                                   \
    do {                                                                             \
        const void *_exp_val = (const void *)(exp);                                  \
        const void *_act_val = (const void *)(act);                                  \
        if (_exp_val != _act_val) {                                                  \
            vf_test_record_failure(__FILE__, __LINE__,                               \
                                   "Expected %s == %s (%p == %p) but they differed", \
                                   #exp, #act, _exp_val, _act_val);                  \
            return;                                                                  \
        }                                                                            \
    } while (0)

#define VF_EXPECT_NE_PTR(exp, act)                                       \
    do {                                                                 \
        const void *_exp_val = (const void *)(exp);                      \
        const void *_act_val = (const void *)(act);                      \
        if (_exp_val == _act_val) {                                      \
            vf_test_record_failure(__FILE__, __LINE__,                   \
                                   "Expected %s != %s but both were %p", \
                                   #exp, #act, _act_val);                \
        }                                                                \
    } while (0)

#define VF_ASSERT_NE_PTR(exp, act)                                       \
    do {                                                                 \
        const void *_exp_val = (const void *)(exp);                      \
        const void *_act_val = (const void *)(act);                      \
        if (_exp_val == _act_val) {                                      \
            vf_test_record_failure(__FILE__, __LINE__,                   \
                                   "Expected %s != %s but both were %p", \
                                   #exp, #act, _act_val);                \
            return;                                                      \
        }                                                                \
    } while (0)

/* Pointer Alignment checks */
#define VF_EXPECT_PTR_ALIGNED(ptr, alignment)                                               \
    do {                                                                                    \
        const void *_p = (const void *)(ptr);                                               \
        size_t      _a = (size_t)(alignment);                                               \
        if (((size_t)_p % _a) != 0) {                                                       \
            vf_test_record_failure(__FILE__, __LINE__,                                      \
                                   "Expected %s to be aligned to %zu bytes, but address %p" \
                                   " has misalignment %zu",                                 \
                                   #ptr, _a, _p, ((size_t)_p % _a));                        \
        }                                                                                   \
    } while (0)

#define VF_ASSERT_PTR_ALIGNED(ptr, alignment)                                               \
    do {                                                                                    \
        const void *_p = (const void *)(ptr);                                               \
        size_t      _a = (size_t)(alignment);                                               \
        if (((size_t)_p % _a) != 0) {                                                       \
            vf_test_record_failure(__FILE__, __LINE__,                                      \
                                   "Expected %s to be aligned to %zu bytes, but address %p" \
                                   " has misalignment %zu",                                 \
                                   #ptr, _a, _p, ((size_t)_p % _a));                        \
            return;                                                                         \
        }                                                                                   \
    } while (0)

/* Memory Equal checks */
#define VF_EXPECT_MEMEQ(exp_ptr, act_ptr, size)                                 \
    do {                                                                        \
        const void *_exp = (const void *)(exp_ptr);                             \
        const void *_act = (const void *)(act_ptr);                             \
        size_t      _sz = (size);                                               \
        if (vf_test_memcmp(_exp, _act, _sz) != 0) {                             \
            vf_test_record_failure(__FILE__, __LINE__,                          \
                                   "Memory mismatch for %s and %s (%zu bytes)", \
                                   #exp_ptr, #act_ptr, _sz);                    \
        }                                                                       \
    } while (0)

#define VF_ASSERT_MEMEQ(exp_ptr, act_ptr, size)                                 \
    do {                                                                        \
        const void *_exp = (const void *)(exp_ptr);                             \
        const void *_act = (const void *)(act_ptr);                             \
        size_t      _sz = (size);                                               \
        if (vf_test_memcmp(_exp, _act, _sz) != 0) {                             \
            vf_test_record_failure(__FILE__, __LINE__,                          \
                                   "Memory mismatch for %s and %s (%zu bytes)", \
                                   #exp_ptr, #act_ptr, _sz);                    \
            return;                                                             \
        }                                                                       \
    } while (0)

/* Memory Zero checks */
#define VF_EXPECT_MEMZERO(ptr, size)                                         \
    do {                                                                     \
        const unsigned char *_p = (const unsigned char *)(ptr);              \
        size_t               _sz = (size);                                   \
        int                  _bad = 0;                                       \
        for (size_t _i = 0; _i < _sz; _i++) {                                \
            if (_p[_i] != 0) {                                               \
                _bad = 1;                                                    \
                break;                                                       \
            }                                                                \
        }                                                                    \
        if (_bad) {                                                          \
            vf_test_record_failure(__FILE__, __LINE__,                       \
                                   "Expected %s (%zu bytes) to be all zero", \
                                   #ptr, _sz);                               \
        }                                                                    \
    } while (0)

#define VF_ASSERT_MEMZERO(ptr, size)                                         \
    do {                                                                     \
        const unsigned char *_p = (const unsigned char *)(ptr);              \
        size_t               _sz = (size);                                   \
        int                  _bad = 0;                                       \
        for (size_t _i = 0; _i < _sz; _i++) {                                \
            if (_p[_i] != 0) {                                               \
                _bad = 1;                                                    \
                break;                                                       \
            }                                                                \
        }                                                                    \
        if (_bad) {                                                          \
            vf_test_record_failure(__FILE__, __LINE__,                       \
                                   "Expected %s (%zu bytes) to be all zero", \
                                   #ptr, _sz);                               \
            return;                                                          \
        }                                                                    \
    } while (0)

/* Bool checks */

#define VF_EXPECT_TRUE(expr)                                                \
    do {                                                                    \
        int _val = !!(expr);                                                \
        if (!_val) {                                                        \
            vf_test_record_failure(__FILE__, __LINE__,                      \
                                   "Expected %s to be true, but was false", \
                                   #expr);                                  \
        }                                                                   \
    } while (0)

#define VF_ASSERT_TRUE(expr)                                                \
    do {                                                                    \
        int _val = !!(expr);                                                \
        if (!_val) {                                                        \
            vf_test_record_failure(__FILE__, __LINE__,                      \
                                   "Expected %s to be true, but was false", \
                                   #expr);                                  \
            return;                                                         \
        }                                                                   \
    } while (0)

#define VF_EXPECT_FALSE(expr)                                               \
    do {                                                                    \
        int _val = !!(expr);                                                \
        if (_val) {                                                         \
            vf_test_record_failure(__FILE__, __LINE__,                      \
                                   "Expected %s to be false, but was true", \
                                   #expr);                                  \
        }                                                                   \
    } while (0)

#define VF_ASSERT_FALSE(expr)                                               \
    do {                                                                    \
        int _val = !!(expr);                                                \
        if (_val) {                                                         \
            vf_test_record_failure(__FILE__, __LINE__,                      \
                                   "Expected %s to be false, but was true", \
                                   #expr);                                  \
            return;                                                         \
        }                                                                   \
    } while (0)

/* Range helpers */

#define VF_EXPECT_IN_RANGE(x, minv, maxv)                                         \
    do {                                                                          \
        double _x = (double)(x);                                                  \
        double _min_v = (double)(minv);                                           \
        double _max_v = (double)(maxv);                                           \
        if (!(_x >= _min_v && _x <= _max_v)) {                                    \
            vf_test_record_failure(__FILE__, __LINE__,                            \
                                   "Expected %s in range [%s, %s], but was %.6f", \
                                   #x, #minv, #maxv, _x);                         \
        }                                                                         \
    } while (0)

#define VF_ASSERT_IN_RANGE(x, minv, maxv)                                         \
    do {                                                                          \
        double _x = (double)(x);                                                  \
        double _min_v = (double)(minv);                                           \
        double _max_v = (double)(maxv);                                           \
        if (!(_x >= _min_v && _x <= _max_v)) {                                    \
            vf_test_record_failure(__FILE__, __LINE__,                            \
                                   "Expected %s in range [%s, %s], but was %.6f", \
                                   #x, #minv, #maxv, _x);                         \
            return;                                                               \
        }                                                                         \
    } while (0)

/* Hex Equality checks */
#define VF_EXPECT_EQ_HEX32(exp, act)                                               \
    do {                                                                           \
        uint32_t _exp_val = (uint32_t)(exp);                                       \
        uint32_t _act_val = (uint32_t)(act);                                       \
        if (_exp_val != _act_val) {                                                \
            vf_test_record_failure(__FILE__, __LINE__,                             \
                                   "Expected 0x%08X, got 0x%08X (expr: %s == %s)", \
                                   _exp_val, _act_val, #exp, #act);                \
        }                                                                          \
    } while (0)

#define VF_ASSERT_EQ_HEX32(exp, act)                                               \
    do {                                                                           \
        uint32_t _exp_val = (uint32_t)(exp);                                       \
        uint32_t _act_val = (uint32_t)(act);                                       \
        if (_exp_val != _act_val) {                                                \
            vf_test_record_failure(__FILE__, __LINE__,                             \
                                   "Expected 0x%08X, got 0x%08X (expr: %s == %s)", \
                                   _exp_val, _act_val, #exp, #act);                \
            return;                                                                \
        }                                                                          \
    } while (0)

#define VF_SKIP(reason)                                    \
    do {                                                   \
        vf_test_record_skip(__FILE__, __LINE__, (reason)); \
        return;                                            \
    } while (0)

/* Force immediate failure */
#define VF_FAIL(msg, ...) \
    VF_ASSERT_MSG(0, msg, __VA_ARGS__)

/* End test early with PASS */
#define VF_PASS() return

extern int vf_test_run(int argc, char **argv);

/* ------------------------------------------
   Macro for test declaration/registering
   if auto-register ENABLED use VF_TEST
   if NOT then VF_TEST and VF_TEST_REGISTER
   ------------------------------------------ */

#if VF_TEST_ENABLE_AUTO_REGISTER
#    if defined(__GNUC__) || defined(__clang__)
#        define VF_TEST(suite, name)                                     \
            static void vf_test_fn__##name(void);                        \
            static void vf_test_reg__##name(void) VF_TEST_CONSTRUCTOR;   \
            static void vf_test_reg__##name(void) {                      \
                vf_test_register_one(#suite, #name, vf_test_fn__##name); \
            }                                                            \
            static void vf_test_fn__##name(void)
#    elif defined(_MSC_VER)
#        define VF_TEST(suite, name)                                                        \
            static void vf_test_fn__##name(void);                                           \
            static int  vf_test_reg__##name(void) {                                         \
                vf_test_register_one(#suite, #name, vf_test_fn__##name);                   \
                return 0;                                                                  \
            }                                                                               \
            VF_TEST_CONSTRUCTOR_SECTION                                                     \
            VF_TEST_CONSTRUCTOR int (*vf_test_reg_ptr__##name)(void) = vf_test_reg__##name; \
            static void vf_test_fn__##name(void)

#    endif
#else
#    define VF_TEST(suite, name) \
        static void vf_test__##suite##__##name(void)
#endif

#define VF_TEST_REGISTER(suite, name) \
    vf_test_register_one(#suite, #name, vf_test__##suite##__##name)

#ifdef __cplusplus
}
#endif

#ifdef VF_TEST_IMPLEMENTATION

#    include <stdarg.h>
#    include <stdio.h>

/* ------------------------------------------
   ANSI Colours
   ------------------------------------------ */
#    if VF_TEST_ENABLE_COLOR
#        define VF_TEST_RED         "\x1b[31m"
#        define VF_TEST_GREEN       "\x1b[32m"
#        define VF_TEST_YELLOW      "\x1b[33m"
#        define VF_TEST_BLUE        "\x1b[34m"
#        define VF_TEST_MAGENTA     "\x1b[35m"
#        define VF_TEST_CYAN        "\x1b[36m"
#        define VF_TEST_BOLD        "\x1b[1m"
#        define VF_TEST_COLOR_RESET "\x1b[0m"
#        define VF_TEST_BG_RED      "\x1b[41m"
#        define VF_TEST_BG_GREEN    "\x1b[42m"
#    else
#        define VF_TEST_RED         ""
#        define VF_TEST_GREEN       ""
#        define VF_TEST_YELLOW      ""
#        define VF_TEST_BLUE        ""
#        define VF_TEST_MAGENTA     ""
#        define VF_TEST_CYAN        ""
#        define VF_TEST_BOLD        ""
#        define VF_TEST_COLOR_RESET ""
#        define VF_TEST_BG_RED      ""
#        define VF_TEST_BG_GREEN    ""
#    endif

/* ------------------------------------------
   Internal Registry
   ------------------------------------------ */

static VFTestCase vf_tests[VF_TEST_MAX_TESTS];
static int        vf_test_count = 0;

void vf_test_register_one(const char *suite, const char *name, PFN_vf_test pfn_fn) {
    if (vf_test_count < VF_TEST_MAX_TESTS) {
        vf_tests[vf_test_count++] = (VFTestCase){suite, name, VF_TEST_RESULT_NONE, 0, pfn_fn};
    } else {
        fprintf(stderr, "Too many tests!\n");
    }
}

/* ------------------------------------------
   Internal Runtime State
   ------------------------------------------ */

typedef struct {
    const char *current_suite_name;
    int         current_test_idx;
    int         total_suite_count;
    int         total_passes;
    int         total_fails;
    int         total_skips;
    uint64_t    total_time_ns;
} VFTestRuntimeState;

/* initialize global runtime state */
static VFTestRuntimeState vf_test_rs = {0};

/* Timing helpers */
#    if defined(_WIN32)
static uint64_t vf_test_time_now_ns(void) {
    vf_test_large_integer freq;
    vf_test_large_integer counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    // scale ticks → nanoseconds
    return (uint64_t)((counter.QuadPart * 1000000000ULL) / freq.QuadPart);
}
#    else
#        include <time.h>
static uint64_t vf_test_time_now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}
#    endif

typedef struct {
    const char *file;
    int         line;
    char        message[512];
} VFTestFailure;

static VFTestFailure vf_test_failures[32];
static int           vf_test_failure_count = 0;

void vf_test_record_failure(const char *file, int line, const char *msg_fmt, ...) {
    if (vf_test_failure_count < 32) {
        VFTestFailure *f = &vf_test_failures[vf_test_failure_count++];
        f->file = file;
        f->line = line;

        va_list args;
        va_start(args, msg_fmt);
        vsnprintf(f->message, sizeof(f->message), msg_fmt, args);
        va_end(args);
    }

    /* Mark test as failed */
    vf_tests[vf_test_rs.current_test_idx].result = VF_TEST_RESULT_FAIL;
}

void vf_test_record_skip(const char *file, int line, const char *reason) {
    (void)file;
    (void)line;
    (void)reason;
    vf_tests[vf_test_rs.current_test_idx].result = VF_TEST_RESULT_SKIP;
}

/* ------------------------------------------
   Sorting Test Cases
   ------------------------------------------ */

static int _test_case_cmp(const void *a, const void *b) {
    const VFTestCase *A = (const VFTestCase *)a;
    const VFTestCase *B = (const VFTestCase *)b;

    int s = vf_test_strcmp(A->suite, B->suite);
    if (s != 0) return s;

    return vf_test_strcmp(A->name, B->name);
}

static void _test_sort(void) {
    for (int i = 1; i < vf_test_count; i++) {
        VFTestCase x = vf_tests[i];
        int        j = i - 1;

        while (j >= 0 && _test_case_cmp(&x, &vf_tests[j]) < 0) {
            vf_tests[j + 1] = vf_tests[j];
            j--;
        }
        vf_tests[j + 1] = x;
    }
}

/* ------------------------------------------
   Symbols for printing
   ------------------------------------------ */

static void _print_time(uint64_t ns) {
    if (ns < 1000ULL) {
        // < 1 µs → nanoseconds
        printf("(%llu ns)", (unsigned long long)ns);
    } else if (ns < 2000000ULL) {
        // < 2 ms → microseconds
        double us = ns / 1000.0;
        printf("(%.2f us)", us);
    } else {
        // otherwise ms
        double ms = ns / 1000000.0;
        printf("(%.2f ms)", ms);
    }
}

static const char *vf_sym_pass = "PASS";
static const char *vf_sym_fail = "FAIL";
static const char *vf_sym_skip = "SKIP";

static void _print_test(const VFTestCase *tc) {
    char namebuf[256];

    /* Left align test name to VF_TEST_NAME_ALIGN */
    snprintf(namebuf, sizeof(namebuf), "%-*s", VF_TEST_NAME_ALIGN, tc->name);

    if (tc->result == VF_TEST_RESULT_SKIP) {
        printf("  %s%s%s %s (skipped)\n",
               VF_TEST_YELLOW,
               vf_sym_skip,
               VF_TEST_COLOR_RESET,
               namebuf);
        return;
    }

    if (tc->result == VF_TEST_RESULT_FAIL) {
        printf("  %s%s%s %s ", VF_TEST_RED, vf_sym_fail, VF_TEST_COLOR_RESET, namebuf);
        _print_time(tc->time_ns);
        printf("\n");

        /* Print failures */
        for (int i = 0; i < vf_test_failure_count; i++) {
            VFTestFailure *f = &vf_test_failures[i];
            printf("      Failure at %s:%d\n", f->file, f->line);
            printf("        %s\n", f->message);
        }

        return;
    }

    /* PASS */
    printf("  %s%s%s %s ", VF_TEST_GREEN, vf_sym_pass, VF_TEST_COLOR_RESET, namebuf);
    _print_time(tc->time_ns);
    printf("\n");
}

static void _print_suite_header(const char *suite) {
    printf("%s%s%s\n",
           VF_TEST_CYAN,
           suite,
           VF_TEST_COLOR_RESET);
}

static void _print_summary(const VFTestRuntimeState *rs) {
    printf("\n=================================================\n");
    printf("                    Test Summary\n");
    printf("-------------------------------------------------\n");
    printf(" Suites:            %d\n", rs->total_suite_count);
    printf(" Tests:             %d total\n",
           rs->total_passes + rs->total_fails + rs->total_skips);
    printf("                    %d passed\n", rs->total_passes);
    printf("                    %d failed\n", rs->total_fails);
    printf("                    %d skipped\n", rs->total_skips);
    printf("\n Total time:        ");
    _print_time(rs->total_time_ns);
    printf("\n");

    printf("\n Status:            %s%s%s\n",
           (rs->total_fails ? VF_TEST_RED : VF_TEST_GREEN),
           (rs->total_fails ? "FAIL" : "PASS"),
           VF_TEST_COLOR_RESET);

    printf("=================================================\n");
}

/* ------------------------------------------
   Test Runner
   ------------------------------------------ */

int vf_test_run(int argc, char **argv) {
    (void)argc;
    (void)argv;

#    if VF_TEST_ENABLE_SORTING
    /* Sort tests */
    _test_sort();
#    endif

    uint64_t total_start = vf_test_time_now_ns();

    for (int i = 0; i < vf_test_count; i++) {
        VFTestCase *tc = &vf_tests[i];

        vf_test_rs.current_test_idx = i;
        vf_test_failure_count = 0;

        /* Print suite header */
        if (!vf_test_rs.current_suite_name || vf_test_strcmp(vf_test_rs.current_suite_name, tc->suite) != 0) {
            vf_test_rs.current_suite_name = tc->suite;
            vf_test_rs.total_suite_count++;
            printf("\n");
            _print_suite_header(tc->suite);
        }

        /* Reset state for this test */
        tc->result = VF_TEST_RESULT_NONE;

        uint64_t start = vf_test_time_now_ns();
        tc->pfn_fn();
        uint64_t end = vf_test_time_now_ns();

        /* Log test case time */
        tc->time_ns = end - start;

        /* In case of no issues, we are setting this to passed */
        if (tc->result == VF_TEST_RESULT_NONE) {
            tc->result = VF_TEST_RESULT_PASS;
        }

        /* Update global counts */
        if (tc->result == VF_TEST_RESULT_SKIP) {
            vf_test_rs.total_skips++;
        } else if (tc->result == VF_TEST_RESULT_FAIL) {
            vf_test_rs.total_fails++;
        } else {
            vf_test_rs.total_passes++;
        }

        /* Print the actual result line(s) */
        _print_test(tc);
    }

    /* Store total time */
    uint64_t total_end = vf_test_time_now_ns();
    vf_test_rs.total_time_ns = total_end - total_start;

    _print_summary(&vf_test_rs);

    return vf_test_rs.total_fails ? 1 : 0;
}

#endif // VF_TEST_IMPLEMENTATION
#endif // VF_TEST_H
