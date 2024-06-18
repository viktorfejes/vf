/*
*   vf_memory - v0.2
*   Header-only tiny memory library.
*
*   RECENT CHANGES:
*       0.2 (2024-06-17)    Added `vf_` prefix to function names;
*                           Improved header-only implementation;
*                           Changed indices to pointers.
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
 */

#ifndef VF_MEMORY_H
#define VF_MEMORY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reimplementation of `memcpy`, so the <string.h> header doesn't
 * need to be included.
 *
 * @param dst Pointer to the destination (copy-to).
 * @param src Pointer to the source (copy-from).
 * @param size Size of data to move.
 * @return The provided `dst` pointer is returned.
 */
extern void* vf_memcpy(void* dst, const void* src, size_t size);

/**
 * @brief Reimplementation of `memset`, so the <string.h> header doesn't
 * need to be included. Sets the first `size` of bytes of the block of memory pointed by
 * `dst` to the specified value (will be cast to unsigned char).
 *
 * @param dst Pointer to the block of memory to fill.
 * @param value Value to be set. `unsigned char` cast will be used to fill.
 * @param size Size of the memory to set (in bytes).
 * @return The provided `dst` pointer is returned.
 */
extern void* vf_memset(void* dst, int value, size_t size);

/**
 * @brief Swap two equal size chunks in memory.
 *
 * @param ptr_a Pointer to one block of memory.
 * @param ptr_b Pointer to another block of memory.
 * @param size Size of the memory to swap in bytes.
 */
extern void vf_memswap(void* ptr_a, void* ptr_b, size_t size);

#ifdef __cplusplus
}
#endif

// End of header file.

#ifdef VF_MEM_IMPLEMENTATION

void* vf_memcpy(void* dst, const void* src, size_t size) {
    unsigned char* d = (unsigned char*)dst;
    const unsigned char* s = (const unsigned char*)src;
    while (size--) {
        *d++ = *s++;
    }
    return dst;
}

void* vf_memset(void* dst, int value, size_t size) {
    unsigned char* d = (unsigned char*)dst;
    while (size--) {
        *d++ = (unsigned char)value;
    }
    return dst;
}

void vf_memswap(void* ptr_a, void* ptr_b, size_t size) {
    unsigned char* a = (unsigned char*)ptr_a;
    unsigned char* b = (unsigned char*)ptr_b;
    unsigned char temp;

    while (size--) {
        temp = *a;
        *a++ = *b;
        *b++ = temp;
    }
}

#endif // VF_MEM_IMPLEMENTATION
#endif // VF_MEMORY_H