#define VF_DARRAY_IMPLEMENTATION
#include "../vf_darray.h"

#include "../vf_test.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

TEST(DynamicArray, Create) {
    int* da = (int*)vf_da_alloc(sizeof(int));
    EXPECT_NE(da, NULL);

    size_t desired_capacity = 10;
    int* da_exact = (int*)vf_da_alloc_exact(desired_capacity, sizeof(int));
    EXPECT_NE(da_exact, NULL);

    size_t da_exact_capacity = vf_da_capacity(da_exact);
    EXPECT_EQ(da_exact_capacity, desired_capacity);

    // Free arrays
    vf_da_free(da_exact);
    vf_da_free(da);

    return true;
}

TEST(DynamicArray, Headers) {
    size_t desired_capacity = 25;
    int* da = (int*)vf_da_alloc_exact(desired_capacity, sizeof(int));
    // Just to make sure check if not null
    EXPECT_NE(da, NULL);

    // Check capacity
    size_t capacity = vf_da_capacity(da);
    EXPECT_EQ(capacity, desired_capacity);

    // Check count
    size_t count = vf_da_count(da);
    EXPECT_EQ(count, 0);

    // Check stride
    size_t stride = vf_da_stride(da);
    EXPECT_EQ(stride, sizeof(int));

    // Check emptiness
    EXPECT_TRUE(vf_da_is_empty(da));

    vf_da_free(da);
    return true;
}

TEST(DynamicArray, Push) {
    // This should create a capacity of 2!
    int* da = (int*)vf_da_alloc(sizeof(int));
    // Just to make sure check if not null
    EXPECT_NE(da, NULL);

    // Push two elements
    int data1 = 8;
    da = (int*)vf_da_push_back(da, &data1);
    int data2 = 2147;
    da = (int*)vf_da_push_back(da, &data2);

    // Check if elements are correct.
    EXPECT_EQ(da[0], data1);
    EXPECT_EQ(da[1], data2);

    // Push more elements to check for resize
    int data3 = 256;  // 2 index
    int data4 = 512;  // 3 index
    int data5 = 1024; // 4 index
    int data6 = 2048; // 5 index
    da = (int*)vf_da_push_back(da, &data3);
    da = (int*)vf_da_push_back(da, &data4);
    da = (int*)vf_da_push_back(da, &data5);
    da = (int*)vf_da_push_back(da, &data6);

    // Check the elements
    EXPECT_EQ(da[2], data3);
    EXPECT_EQ(da[3], data4);
    EXPECT_EQ(da[4], data5);
    EXPECT_EQ(da[5], data6);

    vf_da_free(da);
    return true;
}

TEST(DynamicArray, PopBack) {
    int capacity = 2;
    int* da = (int*)vf_da_alloc_exact(capacity, sizeof(int));

    // Push two elements
    int data1 = 256;
    int data2 = 512;
    da = (int*)vf_da_push_back(da, &data1);
    da = (int*)vf_da_push_back(da, &data2);

    // Pop one back.
    int* popped_el = (int*)vf_da_pop_back(da);
    EXPECT_EQ(data1, *popped_el);

    // It should show one size smaller after pop back.
    EXPECT_EQ(vf_da_count(da), 1);

    vf_da_free(da);
    return true;
}
