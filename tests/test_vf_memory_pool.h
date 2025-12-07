#include "../vf_test.h"

#define VF_MEMORY_POOL_IMPLEMENTATION
#include "../vf_memory_pool.h"

#include <stdbool.h>
#include <stdio.h>

TEST(MemoryPool, Create) {
    size_t block_size = sizeof(int);
    size_t initial_capacity = 8;

    vf_memory_pool_t* pool = vf_memory_pool_create(block_size, initial_capacity);
    EXPECT_NE(pool, NULL);

    size_t aligned_size = (block_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    EXPECT_EQ(pool->block_size, aligned_size);
    EXPECT_EQ(pool->capacity, initial_capacity);
    EXPECT_EQ(pool->used, 0);
    EXPECT_NE(pool->memory, NULL);
    EXPECT_EQ(pool->free_list, NULL);

    vf_memory_pool_destroy(pool);
    return true;
}

TEST(MemoryPool, AllocAndFree) {
    size_t block_size = sizeof(int);
    size_t initial_capacity = 4;

    vf_memory_pool_t* pool = vf_memory_pool_create(block_size, initial_capacity);

    int* data1 = (int*)vf_memory_pool_alloc(pool);
    EXPECT_NE(data1, NULL);
    *data1 = 42;

    int* data2 = (int*)vf_memory_pool_alloc(pool);
    EXPECT_NE(data2, NULL);
    *data2 = 84;

    EXPECT_EQ(pool->used, 2);

    vf_memory_pool_free(pool, data1);
    int* data3 = (int*)vf_memory_pool_alloc(pool);
    EXPECT_EQ(data3, data1);  // Should reuse the freed block
    EXPECT_EQ(pool->used, 2);  // Used count should not increase

    vf_memory_pool_destroy(pool);
    return true;
}

TEST(MemoryPool, Growth) {
    size_t block_size = sizeof(int);
    size_t initial_capacity = 2;

    vf_memory_pool_t* pool = vf_memory_pool_create(block_size, initial_capacity);

    int* data[4];
    for (int i = 0; i < 4; i++) {
        data[i] = (int*)vf_memory_pool_alloc(pool);
        EXPECT_NE(data[i], NULL);
        *data[i] = i;
    }

    EXPECT_GT(pool->capacity, initial_capacity);
    EXPECT_EQ(pool->used, 4);

    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(*data[i], i);
    }

    vf_memory_pool_destroy(pool);
    return true;
}

TEST(MemoryPool, Reset) {
    size_t block_size = sizeof(int);
    size_t initial_capacity = 4;

    vf_memory_pool_t* pool = vf_memory_pool_create(block_size, initial_capacity);

    for (int i = 0; i < 4; i++) {
        int* data = (int*)vf_memory_pool_alloc(pool);
        EXPECT_NE(data, NULL);
    }

    EXPECT_EQ(pool->used, 4);

    vf_memory_pool_reset(pool);
    EXPECT_EQ(pool->used, 0);
    EXPECT_EQ(pool->free_list, NULL);

    int* new_data = (int*)vf_memory_pool_alloc(pool);
    EXPECT_NE(new_data, NULL);
    EXPECT_EQ(pool->used, 1);

    vf_memory_pool_destroy(pool);
    return true;
}

TEST(MemoryPool, StressTest) {
    size_t block_size = sizeof(int);
    size_t initial_capacity = 100;

    vf_memory_pool_t* pool = vf_memory_pool_create(block_size, initial_capacity);

    int* pointers[1000];
    
    // Allocate 1000 blocks
    for (int i = 0; i < 1000; i++) {
        pointers[i] = (int*)vf_memory_pool_alloc(pool);
        EXPECT_NE(pointers[i], NULL);
        *pointers[i] = i;
    }

    EXPECT_GE(pool->capacity, 1000);

    // Free every other block
    for (int i = 0; i < 1000; i += 2) {
        vf_memory_pool_free(pool, pointers[i]);
    }

    // Reallocate
    for (int i = 0; i < 1000; i += 2) {
        pointers[i] = (int*)vf_memory_pool_alloc(pool);
        EXPECT_NE(pointers[i], NULL);
    }

    // Set new values for all blocks
    for (int i = 0; i < 1000; i++) {
        *pointers[i] = i * 2;
    }

    // Verify new data
    for (int i = 0; i < 1000; i++) {
        EXPECT_EQ_INT(*pointers[i], i * 2);
    }

    vf_memory_pool_destroy(pool);
    return true;
}

TEST(MemoryPool, AlignmentTest) {
    struct AlignmentTest {
        char a;
        double b;
        int c;
    };

    size_t block_size = sizeof(struct AlignmentTest);
    size_t initial_capacity = 4;

    vf_memory_pool_t* pool = vf_memory_pool_create(block_size, initial_capacity);

    struct AlignmentTest* data1 = (struct AlignmentTest*)vf_memory_pool_alloc(pool);
    struct AlignmentTest* data2 = (struct AlignmentTest*)vf_memory_pool_alloc(pool);

    EXPECT_NE(data1, NULL);
    EXPECT_NE(data2, NULL);

    // Check alignment
    EXPECT_EQ((uintptr_t)data1 % sizeof(void*), 0);
    EXPECT_EQ((uintptr_t)data2 % sizeof(void*), 0);

    // Check that we can write to all fields without crashing
    data1->a = 'A';
    data1->b = 3.14;
    data1->c = 42;

    data2->a = 'B';
    data2->b = 2.718;
    data2->c = 84;

    // Verify data
    EXPECT_EQ(data1->a, 'A');
    EXPECT_EQ(data1->b, 3.14);
    EXPECT_EQ(data1->c, 42);

    EXPECT_EQ(data2->a, 'B');
    EXPECT_EQ(data2->b, 2.718);
    EXPECT_EQ(data2->c, 84);

    vf_memory_pool_destroy(pool);
    return true;
}
