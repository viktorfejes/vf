/*
*   vf_darray - v0.21
*   Header-only tiny dynamic array implementation.
*
*   RECENT CHANGES:
*       0.21    (2024-06-19)    Removed unnecessary `#pragma once`;
*       0.2     (2024-06-19)    Added `vf_` prefix to function names;
*                               Improved header-only implementation;
*                               Removed non-standard dependencies;
*                               Rewrote `vf_da_resize`;
*                               Added `_vf_memswap` private static function;
*                               Added `_vf_header_set` private static function for convenience;
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
*       - [ ] Add macros for easy type casting.
*       - [ ] Add prefix to enums to avoid collisions.
*       - [ ] Provide macro for malloc for easy user-side swap.
*       - [ ] Revisit resize/reserve...
*
 */

#ifndef VF_DARRAY_H
#define VF_DARRAY_H

#include <stdint.h>
#include <stdbool.h>
// TODO: provide macro for malloc...
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Header enum
// NOTE: DA_MAX_CAPACITY is the field that holds the maximum capacity
// the dynamic array has ever reached. Currently, I believe this will
// help with freeing the right amount of memory in case we shrink
// the darray at one point. This keeps track of that.
enum { DA_STRIDE, DA_COUNT, DA_CAPACITY, DA_MAX_CAPACITY, DA_HEADER_LENGTH };

// Important defines
#define DA_DEFAULT_CAPACITY 2
#define DA_RESIZE_FACTOR    2

/**
 * @brief Function that creates a new Dynamic Array with specified capacity.
 *
 * @param capacity The number of elements that can be held in currently allocated storage.
 * @param stride Size of the element the darray will hold.
 * @return Returns a pointer to the data right after the header.
 */
extern void* vf_da_alloc_exact(size_t capacity, size_t stride);

/**
 * @brief Function to create Dynamic Array at default capacity
 *
 * @param stride Size of the element the darray will hold
 * @return Pointer to the data (right after header)
 * @note Uses `da_alloc_exact` under the hood.
 */
extern void* vf_da_alloc(size_t stride);

/**
 * @brief Frees the memory allocated by the Dynamic Array
 *
 * @param da_data Pointer to the data part of the darray
 */
extern void vf_da_free(void* da_data);

/**
 * @brief Returns the current number of elements of a Dynamic Array
 *
 * @param da_data Pointer to the darray data.
 * @return The number of elements.
 */
extern size_t vf_da_count(const void* da_data);

/**
 * @brief Returns the number of elements that can be held in currently allocated storage
 *
 * @param da_data Pointer to the darray data.
 * @return The capacity of the darray.
 */
extern size_t vf_da_capacity(const void* da_data);

/**
 * @brief Returns size of a single element in bytes.
 *
 * @param da_data Pointer to the darray data.
 * @return size_t The size of a single element in bytes.
 */
extern size_t vf_da_stride(const void* da_data);

/**
 * @brief Checks whether the darray is empty. In this case empty means it has
 * no elements, but there could still be a lot of space allocated. (Check capacity)
 *
 * @param da_data Pointer to the darray data.
 * @return bool `true` if array is empty, `false` if not.
 */
extern bool vf_da_is_empty(const void* da_data);

/**
 * @brief Increases the capacity of a Dynamic Array.
 *
 * @param da_data Pointer to the darray data
 * @param new_capacity The new number of elements the darray should hold.
 * @return void* Returns pointer to the data of the newly allocated chunk.
 * If the new desired capacity is less than or equal to old capacity,
 * no allocation/movement takes place.
 */
extern void* vf_da_reserve(void* da_data, size_t new_capacity);

/**
 * @todo Make sure it does NOTHING if new_capacity == old_capacity
 *
 * @brief Resizes the Dynamic Array to hold `new_capacity` elements.
 * If the current capacity is greater than `new_capacity`, the container
 * will be marked as shrunked in its header, but won't reallocate.
 * If the current capacity is less than `new_capacity`, a new appropriate
 * block will be allocated and the darray will be moved.
 *
 * @param da_data Pointer to the darray data.
 * @param new_capacity New capacity of the container.
 * @return void* Returns darray data.
 */
extern void* vf_da_resize(void* da_data, size_t new_capacity);

/**
 * @brief Adds an element to the end of the array.
 *
 * @param da_data Pointer to the darray data.
 * @param data Pointer to the element to be pushed in.
 * @return The pointer to the darray data.
 */
extern void* vf_da_push_back(void* da_data, const void* data);

/**
 * @brief Remove the last element of the darray.
 * Removing doesn't mean freeing memory, so as long as
 * it's not overwritten, pointers can still reach the element.
 * 
 * @param da_data Pointer to the darray data.
 * @return void* The pointer to the darray data.
 */
extern void* vf_da_pop_back(void* da_data);

/**
 * @brief Inserts an element at a given index into the darray.
 * 
 * @param da_data Pointer to the darray data.
 * @param data Pointer to the data to be inserted into the darray.
 * @param index Index of insertion
 * @return void* The pointer to the darray data.
 */
extern void* vf_da_insert(void* da_data, void* data, size_t index);

extern void vf_da_remove(void* da_data, size_t index);
extern void vf_da_remove_swap(void* da_data, size_t index);

/**
 * @brief Append darray B at the end of darray A.
 * 
 * @warning darray B gets freed at the end of this method!
 * 
 * @param da_data_a Pointer to darray A data.
 * @param da_data_b Pointer to darray B data.
 * @return void* The pointer to darray A data.
 */
extern void* vf_da_append(void* da_data_a, void* da_data_b);

/**
 * @brief Clears the contents of the darray by setting the count to 0.
 *
 * @param da_data Pointer to the darray data.
 */
extern void vf_da_clear(void* da_data);

/**
 * @brief Swaps two elements of a Dynamic Array with one another.
 *
 * @param da_data Pointer to the darray data.
 * @param index_a First index.
 * @param index_b Second index.
 */
extern void vf_da_swap(void* da_data, size_t index_a, size_t index_b);

#define da_foreach()

#ifdef __cplusplus
}
#endif

// End of header file.
#ifdef VF_DARRAY_IMPLEMENTATION

// Memswap implementation...
static void _vf_memswap(void* ptr_a, void* ptr_b, size_t size) {
    unsigned char* a = (unsigned char*)ptr_a;
    unsigned char* b = (unsigned char*)ptr_b;
    unsigned char temp;

    while (size--) {
        temp = *a;
        *a++ = *b;
        *b++ = temp;
    }
}

/**
 * @brief Fetches a field of the header.
 *
 * @private
 *
 * @param da_data Pointer to the darray data.
 * @param field Enum of the field to get from the header.
 * @return The value of the field.
 */
static size_t _vf_da_header_get(const void* da_data, size_t field) {
    size_t* header = (size_t*)da_data - DA_HEADER_LENGTH;
    return header[field];
}

static void _vf_da_header_set(void* array, size_t field, size_t value) {
    size_t* header = (size_t*)array - DA_HEADER_LENGTH;
    header[field] = value;
}

void* vf_da_alloc_exact(size_t capacity, size_t stride) {
    size_t header_size = sizeof(size_t) * DA_HEADER_LENGTH;
    size_t* darray = (size_t*)malloc(header_size + (stride * capacity));

    darray[DA_STRIDE] = stride;
    darray[DA_COUNT] = 0;
    darray[DA_CAPACITY] = capacity;
    darray[DA_MAX_CAPACITY] = capacity;

    return (void*)(darray + DA_HEADER_LENGTH);
}

void* vf_da_alloc(size_t stride) {
    return vf_da_alloc_exact(DA_DEFAULT_CAPACITY, stride);
}

void vf_da_free(void* da_data) {
    size_t* header = (size_t*)da_data - DA_HEADER_LENGTH;
    header[DA_STRIDE] = 0;
    header[DA_CAPACITY] = 0;
    header[DA_COUNT] = 0;
    header[DA_MAX_CAPACITY] = 0;

    free(header);
}

size_t vf_da_count(const void* da_data) {
    return _vf_da_header_get(da_data, DA_COUNT);
}

size_t vf_da_capacity(const void* da_data) {
    size_t* header = (size_t*)da_data - DA_HEADER_LENGTH;
    return header[DA_CAPACITY];
}

size_t vf_da_stride(const void* da_data) {
    size_t* header = (size_t*)da_data - DA_HEADER_LENGTH;
    return header[DA_STRIDE];
}

bool vf_da_is_empty(const void* da_data) {
    return (vf_da_count(da_data) == 0) ? true : false;
}

void* vf_da_reserve(void* da_data, size_t new_capacity) {
    size_t* header = (size_t*)da_data - DA_HEADER_LENGTH;

    if (header[DA_CAPACITY] >= new_capacity) {
        return da_data;
    }

    size_t header_size = sizeof(size_t) * DA_HEADER_LENGTH;
    size_t* new_darray = (size_t*)malloc(header_size + (header[DA_STRIDE] * new_capacity));

    new_darray[DA_STRIDE] = header[DA_STRIDE];
    new_darray[DA_COUNT] = header[DA_COUNT];
    new_darray[DA_CAPACITY] = new_capacity;
    new_darray[DA_MAX_CAPACITY] = new_capacity;

    void* new_da_data = (void*)(new_darray + DA_HEADER_LENGTH);

    memcpy(new_da_data, da_data, header[DA_STRIDE] * header[DA_CAPACITY]);

    return new_da_data;
}

void* vf_da_resize(void* da_data, size_t new_capacity) {
    size_t len = vf_da_count(da_data);
    size_t stride = vf_da_stride(da_data);
    size_t capacity = vf_da_capacity(da_data);

    // If the new capacity is less than the current capacity,
    // the capacity field of the header simply gets changed.
    if (capacity >= new_capacity) {
        capacity = new_capacity;
        return da_data;
    }

    void* new_da = vf_da_alloc_exact(new_capacity, stride);
    memcpy(new_da, da_data, len * stride);
    vf_da_free(da_data);

    return new_da;
}

void* vf_da_push_back(void* da_data, const void* data) {
    size_t len = vf_da_count(da_data);
    size_t stride = vf_da_stride(da_data);
    size_t capacity = vf_da_capacity(da_data);

    // Check if we need to resize the darray
    if (len >= capacity) {
        da_data = vf_da_reserve(da_data, capacity * DA_RESIZE_FACTOR);
    }

    size_t addr = (size_t)da_data;
    addr += (len * stride);
    memcpy((void*)addr, data, stride);
    _vf_da_header_set(da_data, DA_COUNT, len + 1);

    return da_data;
}

void* vf_da_pop_back(void* da_data) {
    // Only do pop_back if the darray is not empty
    if (!vf_da_is_empty(da_data)) {
        // All we need to do is decrement the count
        size_t* header = (size_t*)da_data - DA_HEADER_LENGTH;
        header[DA_COUNT]--;
    }

    return da_data;
}

void* vf_da_insert(void* da_data, void* data, size_t index) {
    size_t* header = (size_t*)da_data - DA_HEADER_LENGTH;
    size_t capacity = vf_da_capacity(da_data);
    size_t stride = vf_da_stride(da_data);
    size_t count = vf_da_count(da_data);

    // Check if we have enough space to insert an element
    // If not, we reserve more
    if (count >= capacity) {
        header[DA_CAPACITY] = capacity * DA_RESIZE_FACTOR;
        da_data = vf_da_reserve(da_data, header[DA_CAPACITY]);
    }

    // Move everything by one
    uint8_t* start = (uint8_t*)da_data + (index * stride);
    size_t length = (count - index) * stride;
    memcpy((void*)(start + stride), start, length);

    // Insert the new element
    memcpy(start, data, stride);

    // Increment the count
    header[DA_COUNT]++;

    return da_data;
}

// void da_remove(void* da_data, size_t index) {}
// void da_remove_swap(void* da_data, size_t index) {}

void* vf_da_append(void* da_data_a, void* da_data_b) {
    size_t* header_a = (size_t*)da_data_a - DA_HEADER_LENGTH;
    size_t* header_b = (size_t*)da_data_b - DA_HEADER_LENGTH;

    // Let's make sure they are both the same stride,
    // otherwise we can't merge them!
    if (header_a[DA_STRIDE] != header_b[DA_STRIDE]) {
        return da_data_a;
    }

    // Check if both arrays can fit into A
    // if not, reserve more space
    // TODO: It might be worth it to reserver slightly more space,
    // otherwise the next push or insert will trigger a reallocation.
    if (header_a[DA_CAPACITY] < (header_a[DA_CAPACITY] + header_b[DA_CAPACITY])) {
        header_a[DA_CAPACITY] = header_a[DA_CAPACITY] + header_b[DA_CAPACITY];
        da_data_a = vf_da_reserve(da_data_a, header_a[DA_CAPACITY]);
    }

    // Copy B into A
    memcpy((void*)((uint8_t*)da_data_a + (header_a[DA_COUNT] * header_a[DA_STRIDE])),
             da_data_b,
             header_a[DA_STRIDE]);

    // Add B's count to A's count
    header_a[DA_COUNT] = header_a[DA_COUNT] + header_b[DA_COUNT];

    // Free B
    vf_da_free(da_data_b);

    return da_data_a;
}

void vf_da_clear(void* da_data) {
    size_t* header = (size_t*)da_data - DA_HEADER_LENGTH;
    header[DA_COUNT] = 0;
}

void vf_da_swap(void* da_data, size_t index_a, size_t index_b) {
    size_t* header = (size_t*)da_data - DA_HEADER_LENGTH;
    _vf_memswap((uint8_t*)da_data + (header[DA_STRIDE] * index_a),
             (uint8_t*)da_data + (header[DA_STRIDE] * index_b),
             header[DA_STRIDE]);
}

#endif // VF_DARRAY_IMPLEMENTATION

#endif // VF_DARRAY_H
