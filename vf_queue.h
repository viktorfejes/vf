/*
*   vf_queue - v0.3
*   Header-only tiny fixed size circular queue library.
*
*   RECENT CHANGES:
*       0.3     (2024-07-31)    Changed pop to return a copy;
*       0.2     (2024-06-19)    Newline added to end of file;
*                               Added `vf_` prefix.;
*                               Fixed bug in `vf_queue_push`;
*                               Changed return type of _back to void*;
*       0.1     (2024-06-18)    Finalized the implementation;
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
*       - [x] Rethink the way push works. Currently it points,
*             and it should copy, I think...
*       - [ ] Push currently silently fails if full. Could be improved...
*       - [x] Maybe Pop should be a copy as well, because the pointer
*             will eventually get invalidated.
*
 */

#ifndef VF_QUEUE_H
#define VF_QUEUE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void* data;
    size_t data_size;
    size_t front;
    size_t back;
    size_t size;
    size_t capacity;
} vf_queue_t;

extern vf_queue_t* vf_queue_create(size_t capacity, size_t data_size);
extern uint8_t vf_queue_empty(vf_queue_t* queue);
extern uint8_t vf_queue_full(vf_queue_t* queue);
extern size_t vf_queue_size(vf_queue_t* queue);
extern void* vf_queue_front(vf_queue_t* queue);
extern void* vf_queue_back(vf_queue_t* queue);
extern void vf_queue_push(vf_queue_t* queue, void* data);
extern void vf_queue_pop(vf_queue_t* queue, void* out_item);
extern void vf_queue_free(vf_queue_t* queue);

#ifdef __cplusplus
}
#endif

// END OF HEADER. -----------------------------------------

#ifdef VF_QUEUE_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

vf_queue_t* vf_queue_create(size_t capacity, size_t data_size) {
    vf_queue_t* queue = (vf_queue_t*)malloc(sizeof(vf_queue_t));
    queue->data = malloc(data_size * capacity);
    queue->data_size = data_size;
    queue->front = -1;
    queue->back = -1;
    queue->size = 0;
    queue->capacity = capacity;

    return queue;
}

uint8_t vf_queue_empty(vf_queue_t* queue) {
    return queue->size <= 0;
}

uint8_t vf_queue_full(vf_queue_t* queue) {
    return queue->size >= queue->capacity;
}

size_t vf_queue_size(vf_queue_t* queue) {
    return queue->size;
}

void* vf_queue_front(vf_queue_t* queue) {
    if (vf_queue_empty(queue)) {
        return NULL;
    }

    return (uint8_t*)queue->data + (queue->data_size * queue->front);
}

void* vf_queue_back(vf_queue_t* queue) {
    return (uint8_t*)queue->data + (queue->data_size * queue->back);
}

void vf_queue_push(vf_queue_t* queue, void* data) {
    // Check if we have space...
    if (vf_queue_full(queue)) {
        return;
    }

    if (vf_queue_empty(queue)) {
        queue->front = 0;
    }

    size_t next_pos = (queue->back + 1) % queue->capacity;
    memcpy((uint8_t*)queue->data + (next_pos * queue->data_size), data, queue->data_size);

    queue->back = next_pos;
    queue->size++;
}

void vf_queue_pop(vf_queue_t* queue, void* out_item) {
    if (vf_queue_empty(queue)) {
        // Queue is empty!
        return;
    }

    // Make a copy of the front item so we don't loose reference
    if (!out_item) return;
    memcpy(out_item, (uint8_t*)queue->data + (queue->data_size * queue->front), queue->data_size);

    if (queue->front == queue->back) {
        queue->front = -1;
        queue->back = -1;
    } else {
        queue->front = (queue->front + 1) % queue->capacity;
    }

    queue->size--;
}

void vf_queue_free(vf_queue_t* queue) {
    free(queue->data);
    free(queue);
}

#endif // VF_QUEUE_IMPLEMENTATION
#endif // VF_QUEUE_H
