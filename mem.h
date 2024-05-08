#include "defines.h"

/**
 * @todo Consider if in this case the types used shouldn't be
 * default types instead of my custom types... (char, int..etc.)
 *
 * @brief Reimplementation of `memcpy`, so the <string.h> header doesn't
 * need to be included.
 *
 * @param dst Pointer to the destination (copy-to).
 * @param src Pointer to the source (copy-from).
 * @param size Size of data to move.
 */
INLINE void mem_copy(void* dst, void* src, u64 size) {
    u8* c_src = (u8*)src;
    u8* c_dst = (u8*)dst;

    for (u64 i = 0; i < size; ++i) { c_dst[i] = c_src[i]; }
}

/**
 * @brief Reimplementation of `memset`, so the <string.h> header doesn't
 * need to be included. Sets the first `size` of bytes of the block of memory pointed by
 * `dst` to the specified value (will be cast to unsigned char).
 *
 * @param dst Pointer to the block of memory to fill.
 * @param value Value to be set. `unsigned char` cast will be used to fill.
 * @param size Size of the memory to set (in bytes).
 * @return The provided `dst` pointer is returned.
 * 
 * @todo decide which loop I want to keep...
 */
INLINE void* mem_set(void* dst, i32 value, u64 size) {
    u8* c_dst = (u8*)dst;
    while (size > 0) {
        *c_dst = (u8)value;
        c_dst++;
        size--;
    }

    return dst;
}

/**
 * @brief Swap two equal size chunks in memory.
 *
 * @param ptr_a Pointer to one block of memory.
 * @param ptr_b Pointer to another block of memory.
 * @param size Size of the memory to swap in bytes.
 */
INLINE void mem_swap(void* ptr_a, void* ptr_b, u64 size) {
    u8* a = (u8*)ptr_a;
    u8* b = (u8*)ptr_b;
    u8 temp;

    for (u64 i = 0; i < size; ++i) {
        temp = a[i];
        a[i] = b[i];
        b[i] = temp;
    }
}