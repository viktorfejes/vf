#pragma once

// Unsigned int types.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

// Signed int types.
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

// Float types
typedef float f32;
typedef double f64;

// Bool types
typedef int b32;
typedef _Bool b8;

// size_t
#if defined(__GNUC__) || defined(__clang__) || defined(__gcc__)
#if defined(__x86_64__) || defined(__ppc64__)
typedef unsigned long long usize;
#define USIZE_SIZE 8
#else
typedef unsigned int usize;
#define USIZE_SIZE 4
#endif
#elif defined(_MSC_VER)
#if defined(_WIN64)
typedef unsigned long long usize;
#define USIZE_SIZE 8
#else
typedef unsigned int usize;
#define USIZE_SIZE 4
#endif
#endif

// Define static asserts
#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// Ensure types are of correct size
STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 byte");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 byte");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 byte");

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 byte");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 byte");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 byte");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 byte");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 byte");

STATIC_ASSERT(sizeof(usize) == USIZE_SIZE, "Expected usize to be 4/8 bytes.");

// Limits
#define U8_MAX  0xff
#define U16_MAX 0xffff
#define U32_MAX 0xffffffffU
#define U64_MAX 0xffffffffffffffffULL

#define true    1
#define false   0
#define nullptr ((void*)0)

// Detect platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PLATFORM_WINDOWS
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_LINUX
#elif defined(__APPLE__)
#define PLATFORM_APPLE
#else
#error "Not supported platform!"
#endif

#define UNUSED __attribute__((unused))
#define PACKED __attribute__((packed))

// Inlining
#ifdef _MSC_VER
#define INLINE        __forceinline
#define ALWAYS_INLINE __forceinline
#define NOINLINE      __declspec(noinline)
#else
#define INLINE        static inline
#define ALWAYS_INLINE static inline __attribute__((always_inline)) UNUSED
#define NOINLINE
#endif