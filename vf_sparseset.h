/*
*   vf_sparseset - v0.1
*   Header-only tiny sparse set library.
*
*   RECENT CHANGES:
*       0.1     (2024-07-31)    Finalized the implementation;
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

#ifndef VF_SPARSESET_H
#define VF_SPARSESET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VF_SPARSESET_INIT_CAPACITY 32
#define VF_SPARSESET_TOMBSTONE 0xffffffffU

typedef struct {
    uint32_t* sparse;
    uint32_t sparse_capacity;
    uint32_t* dense;
    void* data;
    size_t data_size;
    size_t size;
    size_t capacity;
} vf_sparse_set_t;

typedef struct {
    vf_sparse_set_t* set;
    size_t index;
} vf_sparse_set_iterator_t;

extern vf_sparse_set_t* vf_sparse_set_create(size_t capacity, size_t data_size);
extern void vf_sparse_set_destroy(vf_sparse_set_t* set);
extern int vf_sparse_set_insert(vf_sparse_set_t* set, uint32_t id, const void* data);
extern const void* vf_sparse_set_get(vf_sparse_set_t* set, uint32_t id);
extern void* vf_sparse_set_get_mutable(vf_sparse_set_t* set, uint32_t id);
extern int vf_sparse_set_has(vf_sparse_set_t* set, uint32_t id);
extern int vf_sparse_set_set(vf_sparse_set_t* set, uint32_t id, const void* data);
extern void vf_sparse_set_remove(vf_sparse_set_t* set, uint32_t id);

extern vf_sparse_set_iterator_t vf_sparse_set_iterator(vf_sparse_set_t* set);
extern int vf_sparse_set_iterator_next(vf_sparse_set_iterator_t* it, uint32_t* sparse_id, void** data);

#ifdef __cplusplus
}
#endif

// END OF HEADER. -----------------------------------------

#ifdef VF_SPARSESET_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

vf_sparse_set_t* vf_sparse_set_create(size_t capacity, size_t data_size) {
    vf_sparse_set_t* set = (vf_sparse_set_t*)malloc(sizeof(vf_sparse_set_t));
    if (!set) return NULL;

    // If capacity is 1 or less, set it to the default
    set->capacity = capacity <= 1 ? VF_SPARSESET_INIT_CAPACITY : capacity;

    set->size = 0;
    set->sparse_capacity = set->capacity;
    set->data_size = data_size;

    // Initialize sparse, dense and data to the same size
    set->sparse = (uint32_t*)malloc(sizeof(uint32_t) * set->capacity);
    set->dense = (uint32_t*)malloc(sizeof(uint32_t) * set->capacity);
    set->data = malloc(data_size * set->capacity);

    if (!set->sparse || !set->dense || !set->data) {
        free(set->sparse);
        free(set->dense);
        free(set->data);
        free(set);

        return NULL;
    }

    // Set sparse to all TOMBSTONE values for easier invalidation
    memset(set->sparse, VF_SPARSESET_TOMBSTONE, sizeof(uint32_t) * set->capacity);

    return set;
}

void vf_sparse_set_destroy(vf_sparse_set_t* set) {
    if (!set) return;
    free(set->sparse);
    free(set->dense);
    free(set->data);
    free(set);
}

int vf_sparse_set_insert(vf_sparse_set_t* set, uint32_t id, const void* data) {
    // TODO: extract grow functions
    if (id >= set->sparse_capacity) {
        // TODO: check if the maximum size doesn't get too big
        uint32_t new_capacity = id * 2;
        uint32_t* new_sparse = (uint32_t*)malloc(sizeof(uint32_t) * new_capacity);
        if (!new_sparse) return 0; // Allocation failed

        // Copy the existing data to the new place
        memcpy(new_sparse, set->sparse, sizeof(uint32_t) * set->sparse_capacity);

        // Set tombstones to the newly allocated portion of the new memory block
        memset(new_sparse + set->sparse_capacity, VF_SPARSESET_TOMBSTONE, sizeof(uint32_t) * (new_capacity - set->sparse_capacity));

        // Free the old memory
        free(set->sparse);

        // Point to the new memory block
        set->sparse = new_sparse;
        set->sparse_capacity = new_capacity;
    }

    if (set->sparse[id] != VF_SPARSESET_TOMBSTONE) {
        return 0; // Element already exists
    }

    // If the current capacity cannot accomodate more
    if (set->size >= set->capacity) {
        size_t new_capacity = set->capacity * 2;
        // Create new memory blocks for the new capacity
        uint32_t* new_dense = (uint32_t*)malloc(sizeof(uint32_t) * new_capacity);
        void* new_data = malloc(set->data_size * new_capacity);
        if (!new_dense || !new_data) return 0; // Allocation failed

        // Copy the old data to the new place
        memcpy(new_dense, set->dense, sizeof(uint32_t) * set->capacity);
        memcpy(new_data, set->data, set->data_size * set->capacity);

        // Free the old data (is this necessary?)
        free(set->dense);
        free(set->data);

        // Point to the new memory blocks
        set->dense = new_dense;
        set->data = new_data;

        set->capacity = new_capacity;
    }

    set->sparse[id] = set->size;
    set->dense[set->size] = id;
    memcpy((char*)set->data + set->size * set->data_size, data, set->data_size);
    set->size++;

    return 1; // Success
}

const void* vf_sparse_set_get(vf_sparse_set_t* set, uint32_t id) {
    // If id is bigger than our capacity, or there is a tombstone at its place
    // this does NOT exist, return NULL.
    if (id >= set->sparse_capacity || set->sparse[id] == VF_SPARSESET_TOMBSTONE) {
        return NULL;
    }

    return (char*)set->data + set->data_size * set->sparse[id];
}

void* vf_sparse_set_get_mutable(vf_sparse_set_t* set, uint32_t id) {
    // If id is bigger than our capacity, or there is a tombstone at its place
    // this does NOT exist, return NULL.
    if (id >= set->sparse_capacity || set->sparse[id] == VF_SPARSESET_TOMBSTONE) {
        return NULL;
    }

    return (char*)set->data + set->data_size * set->sparse[id];
}

int vf_sparse_set_has(vf_sparse_set_t* set, uint32_t id) {
    if (!set) return 0;
    if (id >= set->sparse_capacity || set->sparse[id] == VF_SPARSESET_TOMBSTONE) {
        return 0;
    }
    return 1;
}

int vf_sparse_set_set(vf_sparse_set_t* set, uint32_t id, const void* data) {
    if (!set) return 0;

    if (id >= set->sparse_capacity || set->sparse[id] == VF_SPARSESET_TOMBSTONE) {
        return 0;
    }

    // To set the memory, let's just copy the data to the existing memory block
    memcpy((char*)set->data + set->data_size * set->sparse[id], data, set->data_size);
    
    return 1;
}

void vf_sparse_set_remove(vf_sparse_set_t* set, uint32_t id) {
    if (id >= set->sparse_capacity || set->sparse[id] == VF_SPARSESET_TOMBSTONE) {
        return;
    }

    uint32_t index = set->sparse[id];
    uint32_t last_id = set->dense[set->size - 1];

    // If it's not the last elements, let's do a swap
    if (index != set->size - 1) {
        set->dense[index] = last_id;
        set->sparse[last_id] = index;
        memcpy((char*)set->data + set->data_size * index,
               (char*)set->data + set->data_size * (set->size - 1),
               set->data_size);
    }

    set->sparse[id] = VF_SPARSESET_TOMBSTONE;
    set->size--;
}

vf_sparse_set_iterator_t vf_sparse_set_iterator(vf_sparse_set_t* set) {
    return (vf_sparse_set_iterator_t) { set, 0 };
}

int vf_sparse_set_iterator_next(vf_sparse_set_iterator_t* it, uint32_t* sparse_id, void** data) {
    if (it->index >= it->set->size) return 0;

    *sparse_id = it->set->dense[it->index];
    *data = (char*)it->set->data + it->set->data_size * it->index;

    it->index++;
    return 1;
}

#endif // VF_SPARSESET_IMPLEMENTATION
#endif // VF_SPARSESET_H
