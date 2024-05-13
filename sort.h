#pragma once

#include "defines.h"
#include "mem.h"

/**
 * @brief 
 * 
 * @param arr 
 * @param low 
 * @param high 
 * @param stride 
 * @param cmp 
 * @return INLINE 
 */
INLINE i32 quicksort_partition(
    void* arr, i32 low, i32 high, u64 stride, i32 (*cmp)(const void*, const void*)) {

    char* pivot = (char*)arr + high * stride;
    i32 i = (low - 1);

    for (i32 j = low; j <= high - 1; j++) {
        if (cmp((char*)arr + j * stride, pivot) < 0) {
            i++;
            mem_swap((char*)arr + i * stride, (char*)arr + j * stride, stride);
        }
    }
    mem_swap((char*)arr + (i + 1) * stride, (char*)arr + high * stride, stride);
    return (i + 1);
}

/**
 * @brief 
 * 
 * @param arr 
 * @param low 
 * @param high 
 * @param stride 
 * @param cmp 
 * @return INLINE 
 */
INLINE
void quicksort(void* arr, i32 low, i32 high, u64 stride, i32 (*cmp)(const void*, const void*)) {
    if (low < high) {
        i32 pi = quicksort_partition(arr, low, high, stride, cmp);
        quicksort(arr, low, pi - 1, stride, cmp);
        quicksort(arr, pi + 1, high, stride, cmp);
    }
}

INLINE void insertsort();
INLINE void heapsort();

INLINE void introsort(void* arr, u64 size, i32 (*cmp)(const void*, const void*)) {
    // TODO
}