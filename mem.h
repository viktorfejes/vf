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
void mem_copy(void* dst, void* src, u64 size);

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
void* mem_set(void* dst, i32 value, u64 size);

/**
 * @brief Swap two equal size chunks in memory.
 *
 * @param ptr_a Pointer to one block of memory.
 * @param ptr_b Pointer to another block of memory.
 * @param size Size of the memory to swap in bytes.
 */
void mem_swap(void* ptr_a, void* ptr_b, u64 size);