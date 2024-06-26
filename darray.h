/**
 * @file darray.h
 * @author Viktor Fejes (viktor@viktorfejes.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "defines.h"
#include "mem.h"

#include <stdlib.h>

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
INLINE void* da_alloc_exact(u64 capacity, u64 stride) {
    u64 header_size = sizeof(u64) * DA_HEADER_LENGTH;
    u64* darray = (u64*)malloc(header_size + (stride * capacity));

    darray[DA_STRIDE] = stride;
    darray[DA_COUNT] = 0;
    darray[DA_CAPACITY] = capacity;
    darray[DA_MAX_CAPACITY] = capacity;

    return (void*)(darray + DA_HEADER_LENGTH);
}

/**
 * @brief Function to create Dynamic Array at default capacity
 *
 * @param stride Size of the element the darray will hold
 * @return Pointer to the data (right after header)
 * @note Uses `da_alloc_exact` under the hood.
 */
INLINE void* da_alloc(u64 stride) {
    return da_alloc_exact(DA_DEFAULT_CAPACITY, stride);
}

/**
 * @brief Frees the memory allocated by the Dynamic Array
 *
 * @param da_data Pointer to the data part of the darray
 */
INLINE void da_free(void* da_data) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    header[DA_STRIDE] = 0;
    header[DA_CAPACITY] = 0;
    header[DA_COUNT] = 0;
    header[DA_MAX_CAPACITY] = 0;

    free(header);
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
INLINE u64 _da_header_get(const void* da_data, u64 field) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    return header[field];
}

/**
 * @brief Returns the current number of elements of a Dynamic Array
 *
 * @param da_data Pointer to the darray data.
 * @return The number of elements.
 */
INLINE u64 da_count(const void* da_data) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    return header[DA_COUNT];
}

/**
 * @brief Returns the number of elements that can be held in currently allocated storage
 *
 * @param da_data Pointer to the darray data.
 * @return The capacity of the darray.
 */
INLINE u64 da_capacity(const void* da_data) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    return header[DA_CAPACITY];
}

/**
 * @brief Returns size of a single element in bytes.
 *
 * @param da_data Pointer to the darray data.
 * @return u64 The size of a single element in bytes.
 */
INLINE u64 da_stride(const void* da_data) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    return header[DA_STRIDE];
}

/**
 * @brief Checks whether the darray is empty. In this case empty means it has
 * no elements, but there could still be a lot of space allocated. (Check capacity)
 *
 * @param da_data Pointer to the darray data.
 * @return bool `true` if array is empty, `false` if not.
 */
INLINE b8 da_is_empty(const void* da_data) {
    return (da_count(da_data) == 0) ? true : false;
}

/**
 * @brief Increases the capacity of a Dynamic Array.
 *
 * @param da_data Pointer to the darray data
 * @param new_capacity The new number of elements the darray should hold.
 * @return void* Returns pointer to the data of the newly allocated chunk.
 * If the new desired capacity is less than or equal to old capacity,
 * no allocation/movement takes place.
 */
INLINE void* da_reserve(void* da_data, u64 new_capacity) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;

    if (header[DA_CAPACITY] >= new_capacity) {
        return da_data;
    }

    u64 header_size = sizeof(u64) * DA_HEADER_LENGTH;
    u64* new_darray = (u64*)malloc(header_size + (header[DA_STRIDE] * new_capacity));

    new_darray[DA_STRIDE] = header[DA_STRIDE];
    new_darray[DA_COUNT] = header[DA_COUNT];
    new_darray[DA_CAPACITY] = new_capacity;
    new_darray[DA_MAX_CAPACITY] = new_capacity;

    void* new_da_data = (void*)(new_darray + DA_HEADER_LENGTH);

    mem_copy(new_da_data, da_data, header[DA_STRIDE] * header[DA_CAPACITY]);

    return new_da_data;
}

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
 * @param default_value The value to initialize the new elements with.
 * @return void* Returns darray data.
 */
INLINE void* da_resize(void* da_data, u64 new_capacity, i32 default_value) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    // If the new capacity is less than the current capacity,
    // the capacity field of the header simply gets changed.
    // However, the DA_MAX_CAPACITY field remains the same
    // to keep track of the maximum size just in case.
    if (header[DA_CAPACITY] >= new_capacity) {
        header[DA_CAPACITY] = new_capacity;
        return da_data;
    }

    u64 header_size = sizeof(u64) * DA_HEADER_LENGTH;
    u64* new_darray = (u64*)malloc(header_size + (header[DA_STRIDE] * new_capacity));

    new_darray[DA_STRIDE] = header[DA_STRIDE];
    new_darray[DA_COUNT] = header[DA_COUNT];
    new_darray[DA_CAPACITY] = new_capacity;
    new_darray[DA_MAX_CAPACITY] = new_capacity;

    void* new_da_data = (void*)(new_darray + DA_HEADER_LENGTH);

    mem_set(new_da_data, default_value, header[DA_STRIDE] * new_capacity);
    mem_copy(new_da_data, da_data, header[DA_STRIDE] * header[DA_CAPACITY]);

    return new_da_data;
}

/**
 * @brief Adds an element to the end of the array.
 *
 * @param da_data Pointer to the darray data.
 * @param data Pointer to the element to be pushed in.
 * @return The pointer to the darray data.
 */
INLINE void* da_push_back(void* da_data, const void* data) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;

    // Check if we need to resize the darray
    if (header[DA_COUNT] >= header[DA_CAPACITY]) {
        da_data = da_reserve(da_data, header[DA_CAPACITY] * DA_RESIZE_FACTOR);
    }

    u64 addr = (u64)da_data;
    addr += (header[DA_COUNT] * header[DA_STRIDE]);
    mem_copy((void*)addr, data, header[DA_STRIDE]);

    header[DA_COUNT]++;

    return da_data;
}

/**
 * @brief Remove the last element of the darray.
 * Removing doesn't mean freeing memory, so as long as
 * it's not overwritten, pointers can still reach the element.
 * 
 * @param da_data Pointer to the darray data.
 * @return void* The pointer to the darray data.
 */
INLINE void* da_pop_back(void* da_data) {
    // Only do pop_back if the darray is not empty
    if (!da_is_empty(da_data)) {
        // All we need to do is decrement the count
        u64* header = (u64*)da_data - DA_HEADER_LENGTH;
        header[DA_COUNT]--;
    }

    return da_data;
}

/**
 * @brief Inserts an element at a given index into the darray.
 * 
 * @param da_data Pointer to the darray data.
 * @param data Pointer to the data to be inserted into the darray.
 * @param index Index of insertion
 * @return void* The pointer to the darray data.
 */
INLINE void* da_insert(void* da_data, void* data, u64 index) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    u64 capacity = da_capacity(da_data);
    u64 stride = da_stride(da_data);
    u64 count = da_count(da_data);

    // Check if we have enough space to insert an element
    // If not, we reserve more
    if (count >= capacity) {
        header[DA_CAPACITY] = capacity * DA_RESIZE_FACTOR;
        da_data = da_reserve(da_data, header[DA_CAPACITY]);
    }

    // Move everything by one
    u8* start = (u8*)da_data + (index * stride);
    u64 length = (count - index) * stride;
    mem_copy((void*)(start + stride), start, length);

    // Insert the new element
    mem_copy(start, data, stride);

    // Increment the count
    header[DA_COUNT]++;

    return da_data;
}

INLINE void da_remove(void* da_data, u64 index);
INLINE void da_remove_swap(void* da_data, u64 index);

/**
 * @brief Append darray B at the end of darray A.
 * 
 * @warning darray B gets freed at the end of this method!
 * 
 * @param da_data_a Pointer to darray A data.
 * @param da_data_b Pointer to darray B data.
 * @return void* The pointer to darray A data.
 */
INLINE void* da_append(void* da_data_a, void* da_data_b) {
    u64* header_a = (u64*)da_data_a - DA_HEADER_LENGTH;
    u64* header_b = (u64*)da_data_b - DA_HEADER_LENGTH;

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
        da_data_a = da_reserve(da_data_a, header_a[DA_CAPACITY]);
    }

    // Copy B into A
    mem_copy((void*)((u8*)da_data_a + (header_a[DA_COUNT] * header_a[DA_STRIDE])),
             da_data_b,
             header_a[DA_STRIDE]);

    // Add B's count to A's count
    header_a[DA_COUNT] = header_a[DA_COUNT] + header_b[DA_COUNT];

    // Free B
    da_free(da_data_b);

    return da_data_a;
}

/**
 * @brief Clears the contents of the darray by setting the count to 0.
 *
 * @param da_data Pointer to the darray data.
 */
INLINE void da_clear(void* da_data) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    header[DA_COUNT] = 0;
}

/**
 * @brief Swaps two elements of a Dynamic Array with one another.
 *
 * @param da_data Pointer to the darray data.
 * @param index_a First index.
 * @param index_b Second index.
 */
INLINE void da_swap(void* da_data, u64 index_a, u64 index_b) {
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    mem_swap(da_data + (header[DA_STRIDE] * index_a),
             da_data + (header[DA_STRIDE] * index_b),
             header[DA_STRIDE]);
}

#define da_foreach()

#ifdef __cplusplus
}
#endif