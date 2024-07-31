/*
*   vf_binaryheap - v0.1
*   Header-only tiny binary heap library.
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

#ifndef VF_BINARYHEAP_H
#define VF_BINARYHEAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct vf_binaryheap {
    void* data;
    size_t data_size;
    int* priorities;
    size_t size;
    size_t capacity;
    int is_min_heap;
} vf_binaryheap_t;

typedef void (*vf_bh_traverse_callback)(void* item, int priority, void* user_data);

extern vf_binaryheap_t* vf_bh_create(size_t capacity, size_t data_size, int is_min_heap);
extern void vf_bh_destroy(vf_binaryheap_t* heap);
extern void vf_bh_clear(vf_binaryheap_t* heap);
extern void vf_bh_push(vf_binaryheap_t* heap, void* item, int priority);
extern void vf_bh_pop(vf_binaryheap_t* heap, void* out_item);
extern void* vf_bh_top(vf_binaryheap_t* heap);
extern void vf_bh_update_priority(vf_binaryheap_t* heap, void* item, int new_priority);
extern size_t vf_bh_size(vf_binaryheap_t* heap);
extern int vf_bh_empty(vf_binaryheap_t* heap);
extern void vf_bh_traverse(vf_binaryheap_t* heap, vf_bh_traverse_callback callback, void* user_data);

#ifdef __cplusplus
}
#endif

// END OF HEADER. -----------------------------------------

#ifdef VF_BINARYHEAP_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

static size_t _parent(size_t i) {
    return (i - 1) / 2;
}

static size_t _left_child(size_t i) {
    return (i << 1) + 1;
}

static int _compare(vf_binaryheap_t* heap, int a, int b) {
    return heap->is_min_heap ? a > b : a < b;
}

static void _sift_up(vf_binaryheap_t* heap, size_t i) {
    void* temp_item = malloc(heap->data_size);
    if (!temp_item) return; // Allocation failure

    // Copy the item to a temporary buffer
    memcpy(temp_item, (char*)heap->data + heap->data_size * i, heap->data_size);

    const int priority = heap->priorities[i];

    // Sift up
    while (i > 0) {
        const size_t parent_index = _parent(i);
        if (!_compare(heap, heap->priorities[parent_index], priority)) break;

        // Move the parent down
        memcpy(
            (char*)heap->data + heap->data_size * i,
            (char*)heap->data + heap->data_size * parent_index,
            heap->data_size
        );
        heap->priorities[i] = heap->priorities[parent_index];
        i = parent_index;
    }

    // Place the original item in its final position
    memcpy(
        (char*)heap->data + heap->data_size * i,
        temp_item,
        heap->data_size
    );
    heap->priorities[i] = priority;

    free(temp_item);
}

static void _sift_down(vf_binaryheap_t* heap, size_t i) {
    void* temp_item = malloc(heap->data_size);
    if (!temp_item) return; // Allocation failure
    memcpy(temp_item, (char*)heap->data + heap->data_size * i, heap->data_size);
    
    const int priority = heap->priorities[i];
    const size_t half_size = heap->size >> 1;

    while (i < half_size) {
        size_t child = _left_child(i);
        size_t right_child = child + 1;

        if (right_child < heap->size && _compare(heap, heap->priorities[child], heap->priorities[right_child])) {
            child = right_child;
        }

        if (!_compare(heap, priority, heap->priorities[child])) break;

        memcpy(
            (char*)heap->data + heap->data_size * i,
            (char*)heap->data + heap->data_size * child,
            heap->data_size
        );
        heap->priorities[i] = heap->priorities[child];
        i = child;
    }

    memcpy(
        (char*)heap->data + heap->data_size * i,
        temp_item,
        heap->data_size
    );
    heap->priorities[i] = priority;

    free(temp_item);
}

static void _jump_and_insert(vf_binaryheap_t* heap, void* item, int priority) {
    if (heap->size == heap->capacity) {
        // Heap is full!
        return;
    }

    size_t i = heap->size;

    // Jump phase
    while (i > 0) {
        const size_t parent_index = _parent(i);

        if (!_compare(heap, heap->priorities[parent_index], priority)) break;

        memcpy(
            (char*)heap->data + heap->data_size * i,
            (char*)heap->data + heap->data_size * parent_index,
            heap->data_size
        );

        heap->priorities[i] = heap->priorities[parent_index];
        i = parent_index;
    }

    // Push phase
    memcpy(
        (char*)heap->data + heap->data_size * i,
        item,
        heap->data_size
    );
    heap->priorities[i] = priority;
    heap->size++;
}

vf_binaryheap_t* vf_bh_create(size_t capacity, size_t data_size, int is_min_heap) {
    vf_binaryheap_t* heap = (vf_binaryheap_t*)malloc(sizeof(vf_binaryheap_t));
    heap->capacity = capacity;
    heap->size = 0;
    heap->is_min_heap = is_min_heap;

    heap->data = malloc(data_size * capacity);
    heap->data_size = data_size;
    // memset(heap->data, 0, data_size * capacity);

    heap->priorities = (int*)malloc(sizeof(int) * capacity);

    return heap;
}

void vf_bh_destroy(vf_binaryheap_t* heap) {
    if (heap) {
        if (heap->priorities) {
            free(heap->priorities);
        }
        if (heap->data) {
            free(heap->data);
        }

        free(heap);
    }
}

void vf_bh_clear(vf_binaryheap_t* heap) {
    memset(heap->data, 0, heap->data_size * heap->size);
    memset(heap->priorities, 0, sizeof(int) * heap->size);
    heap->size = 0;
}

void vf_bh_push(vf_binaryheap_t* heap, void* item, int priority) {
    _jump_and_insert(heap, item, priority);
}

void vf_bh_pop(vf_binaryheap_t* heap, void* out_item) {
    if (heap->size == 0) {
        // Heap is empty!
        return;
    }

    // Make a copy of the top
    if (!out_item) return;
    memcpy(out_item, heap->data, heap->data_size);

    // Replace top
    memcpy(
        heap->data,
        (char*)heap->data + heap->data_size * (heap->size - 1),
        heap->data_size
    );
    heap->priorities[0] = heap->priorities[heap->size - 1];
    heap->size--;

    _sift_down(heap, 0);
}

void* vf_bh_top(vf_binaryheap_t* heap) {
    if (heap->size == 0) {
        // Heap is empty!
        return NULL;
    }

    return heap->data;
}

void vf_bh_update_priority(vf_binaryheap_t* heap, void* item, int new_priority) {
    size_t i = 0;
    while (i < heap->size) {
        if (memcmp((const char*)heap->data + heap->data_size * i, (const char*)item, heap->data_size) == 0) {
            break;
        }
        i++;
    }
    if (i == heap->size) {
        // Item not found in queue!
        return;
    }

    const int old_priority = heap->priorities[i];
    heap->priorities[i] = new_priority;

    if (_compare(heap, old_priority, new_priority)) {
        _sift_up(heap, i);
    } else {
        _sift_down(heap, i);
    }
}

size_t vf_bh_size(vf_binaryheap_t* heap) {
    return heap->size;
}

int vf_bh_empty(vf_binaryheap_t* heap) {
    return !heap->size;
}

void vf_bh_traverse(vf_binaryheap_t* heap, vf_bh_traverse_callback callback, void* user_data) {
    for (size_t i = 0; i < heap->size; i++) {
        callback((char*)heap->data + i * heap->data_size, heap->priorities[i], user_data);
    }
}

#endif // VF_BINARYHEAP_IMPLEMENTATION
#endif // VF_BINARYHEAP_H
