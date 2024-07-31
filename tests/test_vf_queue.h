#include "../vf_test.h"

#define VF_QUEUE_IMPLEMENTATION
#include "../vf_queue.h"

#include <stdbool.h>
#include <stdio.h>

TEST(Queue, Create) {
    size_t capacity = 8;

    // Create a queue
    vf_queue_t* queue = vf_queue_create(capacity, sizeof(int));
    EXPECT_NE(queue, NULL);

    // Check if queue is empty as it should be
    EXPECT_TRUE(vf_queue_empty(queue));

    // Check if queue is NOT full as it should be
    EXPECT_FALSE(vf_queue_full(queue));

    // Check if size is 0 as it should be
    EXPECT_EQ(vf_queue_size(queue), 0);

    // Check if capacity is `capacity`
    EXPECT_EQ(queue->capacity, capacity);

    vf_queue_free(queue);
    return true;
}

TEST(Queue, Push) {
    size_t init_capacity = 6;
    vf_queue_t* queue = vf_queue_create(init_capacity, sizeof(int));

    // Test push
    int data1 = 21;
    vf_queue_push(queue, &data1);
    // See if size is ok
    EXPECT_EQ(vf_queue_size(queue), 1);
    // See if front is equal to data1
    EXPECT_EQ(*(int*)vf_queue_front(queue), data1);
    // Since we only have a single member, back == front
    EXPECT_EQ(*(int*)vf_queue_back(queue), *(int*)vf_queue_front(queue));

    // Push more in
    int data2 = 9;
    int data3 = 15;
    vf_queue_push(queue, &data2);
    vf_queue_push(queue, &data3);
    // Check front... (it should still be the first data (data1))
    EXPECT_EQ(*(int*)vf_queue_front(queue), data1);

    // ...and back (should be the most recent push - data3)
    EXPECT_EQ(*(int*)vf_queue_back(queue), data3);

    // See if we can push in more than capacity
    vf_queue_push(queue, &data2);
    vf_queue_push(queue, &data2);
    vf_queue_push(queue, &data2);
    EXPECT_TRUE(vf_queue_full(queue));
    // This should be more than capacity!
    vf_queue_push(queue, &data2);
    EXPECT_EQ(vf_queue_size(queue), init_capacity);

    vf_queue_free(queue);
    return true;
}

TEST(Queue, Pop) {
    size_t init_capacity = 3;
    vf_queue_t* queue = vf_queue_create(init_capacity, sizeof(int));

    int data1 = 1;
    int data2 = 2;
    int data3 = 4;
    vf_queue_push(queue, &data1);
    vf_queue_push(queue, &data2);
    vf_queue_push(queue, &data3);
    
    // Queue should be full here...
    EXPECT_TRUE(vf_queue_full(queue));

    int popped_data = *(int*)vf_queue_pop(queue);
    EXPECT_EQ(popped_data, data1);
    EXPECT_FALSE(vf_queue_full(queue));
    EXPECT_EQ(vf_queue_size(queue), 2);

    vf_queue_free(queue);
    return true;
}
