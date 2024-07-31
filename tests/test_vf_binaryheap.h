#include "../vf_test.h"

#define VF_BINARYHEAP_IMPLEMENTATION
#include "../vf_binaryheap.h"

#include <stdbool.h>
#include <stdio.h>

TEST(BinaryHeap, Create) {
    size_t capacity = 8;

    // Create a max heap
    vf_binaryheap_t* max_heap = vf_bh_create(capacity, sizeof(int), false);
    EXPECT_NE(max_heap, NULL);

    // Check if heap is empty as it should be
    EXPECT_TRUE(vf_bh_empty(max_heap));

    // Check if size is 0 as it should be
    EXPECT_EQ(vf_bh_size(max_heap), 0);

    // Check if capacity is `capacity`
    EXPECT_EQ(max_heap->capacity, capacity);

    // Create a min heap
    vf_binaryheap_t* min_heap = vf_bh_create(capacity, sizeof(int), true);
    EXPECT_NE(min_heap, NULL);
    EXPECT_TRUE(min_heap->is_min_heap);

    vf_bh_destroy(max_heap);
    vf_bh_destroy(min_heap);
    return true;
}

TEST(BinaryHeap, PushMax) {
    size_t capacity = 5;
    vf_binaryheap_t* heap = vf_bh_create(capacity, sizeof(int), false);

    // Test push
    int data[] = {5, 10, 3, 8, 2};
    for (int i = 0; i < 5; i++) {
        vf_bh_push(heap, &data[i], data[i]);
        EXPECT_EQ(vf_bh_size(heap), i + 1);
    }
    EXPECT_EQ(*(int*)vf_bh_top(heap), 10);

    // Try to push when heap is full
    int extra = 7;
    vf_bh_push(heap, &extra, extra);
    EXPECT_EQ(vf_bh_size(heap), capacity);

    vf_bh_destroy(heap);
    return true;
}

TEST(BinaryHeap, PushMin) {
    size_t capacity = 5;
    vf_binaryheap_t* heap = vf_bh_create(capacity, sizeof(int), true);

    // Test push
    int data[] = {5, 10, 3, 8, 2};
    for (int i = 0; i < 5; i++) {
        vf_bh_push(heap, &data[i], data[i]);
        EXPECT_EQ(vf_bh_size(heap), i + 1);
    }

    // Check top (should be the smallest value)
    EXPECT_EQ(*(int*)vf_bh_top(heap), 2);

    vf_bh_destroy(heap);
    return true;
}

TEST(BinaryHeap, PopMax) {
    vf_binaryheap_t* heap = vf_bh_create(5, sizeof(int), false);

    int data[] = {5, 10, 3, 8, 2};
    for (int i = 0; i < 5; i++) {
        vf_bh_push(heap, &data[i], data[i]);
    }

    // Pop and check values
    int expected[] = {10, 8, 5, 3, 2};
    for (int i = 0; i < 5; i++) {
        int popped;
        vf_bh_pop(heap, &popped);
        EXPECT_EQ(popped, expected[i]);
        EXPECT_EQ(vf_bh_size(heap), 4 - i);
    }

    // Check if heap is empty after all pops
    EXPECT_TRUE(vf_bh_empty(heap));

    vf_bh_destroy(heap);
    return true;
}

TEST(BinaryHeap, PopMin) {
    vf_binaryheap_t* heap = vf_bh_create(5, sizeof(int), true);

    int data[] = {5, 10, 3, 8, 2};
    for (int i = 0; i < 5; i++) {
        vf_bh_push(heap, &data[i], data[i]);
    }

    // Pop and check values
    int expected[] = {2, 3, 5, 8, 10};
    for (int i = 0; i < 5; i++) {
        int popped;
        vf_bh_pop(heap, &popped);
        EXPECT_EQ(popped, expected[i]);
    }

    vf_bh_destroy(heap);
    return true;
}

TEST(BinaryHeap, UpdatePriority) {
    vf_binaryheap_t* heap = vf_bh_create(5, sizeof(int), false);

    int data[] = {5, 10, 3, 8, 2};
    for (int i = 0; i < 5; i++) {
        vf_bh_push(heap, &data[i], data[i]);
    }

    // Update priority of 3 to 15
    vf_bh_update_priority(heap, &data[2], 15);

    // Check top (should now be 3 with priority 15)
    EXPECT_EQ(*(int*)vf_bh_top(heap), 3);

    vf_bh_destroy(heap);
    return true;
}
