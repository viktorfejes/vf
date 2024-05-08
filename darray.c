#include "darray.h"

#include <stdlib.h>

void* da_alloc_exact(u64 capacity, u64 stride)
{
    u64 header_size = sizeof(u64) * DA_HEADER_LENGTH;
    u64* darray = malloc(header_size + (stride * capacity));

    darray[DA_STRIDE] = stride;
    darray[DA_COUNT] = 0;
    darray[DA_CAPACITY] = capacity;
    darray[DA_MAX_CAPACITY] = capacity;

    return (void*)(darray + DA_HEADER_LENGTH);
}

void* da_alloc(u64 stride)
{
    return da_alloc_exact(DA_DEFAULT_CAPACITY, stride);
}

void* da_push_back(void* da_data, const void* data)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;

    // Check if we need to resize the darray
    if (header[DA_COUNT] >= header[DA_CAPACITY])
    {
        da_data = da_reserve(da_data, header[DA_CAPACITY] * DA_RESIZE_FACTOR);
    }

    u64 addr = (u64)da_data;
    addr += (header[DA_COUNT] * header[DA_STRIDE]);
    mem_copy((void*)addr, data, header[DA_STRIDE]);

    return da_data;
}

void da_clear(void* da_data)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    header[DA_COUNT] = 0;
}

void* da_reserve(void* da_data, u64 new_capacity)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;

    if (header[DA_CAPACITY] >= new_capacity)
    {
        return da_data;
    }

    u64 header_size = sizeof(u64) * DA_HEADER_LENGTH;
    u64* new_darray = malloc(header_size + (header[DA_STRIDE] * new_capacity));

    new_darray[DA_STRIDE] = header[DA_STRIDE];
    new_darray[DA_COUNT] = header[DA_COUNT];
    new_darray[DA_CAPACITY] = new_capacity;
    new_darray[DA_MAX_CAPACITY] = new_capacity;

    void* new_da_data = (void*)(new_darray + DA_HEADER_LENGTH);

    mem_copy(new_da_data, da_data, header[DA_STRIDE] * header[DA_CAPACITY]);

    return new_da_data;
}

void* da_resize(void* da_data, u64 new_capacity, i32 default_value)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    // If the new capacity is less than the current capacity,
    // the capacity field of the header simply gets changed.
    // However, the DA_MAX_CAPACITY field remains the same
    // to keep track of the maximum size just in case.
    if (header[DA_CAPACITY] >= new_capacity)
    {
        header[DA_CAPACITY] = new_capacity;
        return da_data;
    }

    u64 header_size = sizeof(u64) * DA_HEADER_LENGTH;
    u64* new_darray = malloc(header_size + (header[DA_STRIDE] * new_capacity));

    new_darray[DA_STRIDE] = header[DA_STRIDE];
    new_darray[DA_COUNT] = header[DA_COUNT];
    new_darray[DA_CAPACITY] = new_capacity;
    new_darray[DA_MAX_CAPACITY] = new_capacity;

    void* new_da_data = (void*)(new_darray + DA_HEADER_LENGTH);

    mem_set(new_da_data, default_value, header[DA_STRIDE] * new_capacity);
    mem_copy(new_da_data, da_data, header[DA_STRIDE] * header[DA_CAPACITY]);

    return new_da_data;
}

void da_swap(void* da_data, u64 index_a, u64 index_b)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    mem_swap(da_data + (header[DA_STRIDE] * index_a), da_data + (header[DA_STRIDE] * index_b),
             header[DA_STRIDE]);
}

u64 _da_header_get(const void* da_data, u64 field)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    return header[field];
}

u64 da_count(const void* da_data)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    return header[DA_COUNT];
}

u64 da_capacity(const void* da_data)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    return header[DA_CAPACITY];
}

u64 da_stride(const void* da_data)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    return header[DA_STRIDE];
}

b8 da_is_empty(const void* da_data)
{
    return (da_count(da_data) == 0) ? true : false;
}

void da_free(void* da_data)
{
    u64* header = (u64*)da_data - DA_HEADER_LENGTH;
    // u64 header_size = sizeof(u64) * DA_HEADER_LENGTH;
    // u64 total_size = header_size + (header[DA_MAX_CAPACITY] * header[DA_STRIDE]);
    free(header);
}