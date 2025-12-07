/*
*   vf_memory_pool - v0.1
*   Header-only tiny memory pool data structure.
*
*   RECENT CHANGES:
*       0.1     (2024-08-04)    Finalized the implementation;
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

#ifndef VF_MEMORY_POOL_H
#define VF_MEMORY_POOL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vf_memory_pool_t vf_memory_pool_t;

extern vf_memory_pool_t* vf_memory_pool_create(size_t block_size, size_t initial_capacity);
extern void vf_memory_pool_destroy(vf_memory_pool_t* pool);
extern void* vf_memory_pool_alloc(vf_memory_pool_t* pool);
extern void vf_memory_pool_free(vf_memory_pool_t* pool, void* ptr);
extern void vf_memory_pool_reset(vf_memory_pool_t* pool);

#ifdef __cplusplus
}
#endif

// END OF HEADER. -----------------------------------------

#ifdef VF_MEMORY_POOL_IMPLEMENTATION

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define VF_MEMORY_POOL_INITIAL_CAPACITY 64

struct vf_memory_pool_t {
    void* raw_address;
    void* memory;
    size_t block_size;
    size_t capacity;
    size_t used;
    void* free_list;
};

static bool _grow_pool(vf_memory_pool_t* pool) {
    // Cannot grow further without risking overflow
    if (pool->capacity > SIZE_MAX / 2) return false;
    
    // Growth by 50%
    size_t new_capacity = pool->capacity + (pool->capacity >> 1);

    // Grab a new chunk that's slightly bigger than we need so we can align
    size_t total_size = pool->block_size * new_capacity + sizeof(void*) - 1;
    void* new_raw_memory = malloc(total_size);
    if (!new_raw_memory) return false;

    // Align address
    uintptr_t new_aligned_address = ((uintptr_t)new_raw_memory + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    // Calculate offset between old and new aligned addresses
    ptrdiff_t offset = new_aligned_address - (uintptr_t)pool->memory;

    // Copy current data to aligned address
    memcpy((void*)new_aligned_address, pool->memory, pool->block_size * pool->used);

    // Update free list pointers
    void** current = &pool->free_list;
    size_t safety_counter = 0;
    while(*current && safety_counter < pool->capacity) {
        *current = (void*)((uint8_t*)*current + offset);
        current = (void**)*current;
        safety_counter++;
    }

    // Free the old RAW memory
    free(pool->raw_address);

    // Point to the correct memory
    pool->raw_address = new_raw_memory;
    pool->memory = (void*)new_aligned_address;
    pool->capacity = new_capacity;

    return true;
}

vf_memory_pool_t* vf_memory_pool_create(size_t block_size, size_t initial_capacity) {
    vf_memory_pool_t* pool = (vf_memory_pool_t*)malloc(sizeof(vf_memory_pool_t));
    if (!pool) return NULL;

    size_t aligned_size = (block_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    pool->block_size = aligned_size;
    pool->capacity = initial_capacity <= 1 ? VF_MEMORY_POOL_INITIAL_CAPACITY : initial_capacity;
    pool->used = 0;

    // Over-allocate memory by sizeof(void*) - 1 bytes
    size_t total_size = pool->block_size * pool->capacity + sizeof(void*) - 1;
    void* raw_memory = malloc(total_size);
    if (!raw_memory) {
        free(pool);
        return NULL;
    }

    // Align the memory address
    uintptr_t aligned_address = ((uintptr_t)raw_memory + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    pool->memory = (void*)aligned_address;

    // Store the raw address for freeing
    pool->raw_address = raw_memory;

    pool->free_list = NULL;
    return pool;
}

void vf_memory_pool_destroy(vf_memory_pool_t* pool) {
    if (pool) {
        // Free the original raw memory
        free(pool->raw_address);
        free(pool);
    }
}

void* vf_memory_pool_alloc(vf_memory_pool_t* pool) {
    if (pool->free_list) {
        void* ptr = pool->free_list;
        pool->free_list = *(void**)pool->free_list;
        return ptr;
    }

    // Hopefully, we only get here occasionally,
    // as this is quite costly because of the free list
    if (pool->used >= pool->capacity) {
        if (!_grow_pool(pool)) return NULL;
    }

    void* ptr = (char*)pool->memory + (pool->used * pool->block_size);
    pool->used++;
    return ptr;
}

void vf_memory_pool_free(vf_memory_pool_t* pool, void* ptr) {
    void** free_block = (void**)ptr;
    *free_block = pool->free_list;
    pool->free_list = free_block;
}

void vf_memory_pool_reset(vf_memory_pool_t* pool) {
    pool->used = 0;
    pool->free_list = NULL;
}

#endif // VF_MEMORY_POOL_IMPLEMENTATION
#endif // VF_MEMORY_POOL_H
