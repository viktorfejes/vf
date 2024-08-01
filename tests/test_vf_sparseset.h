#include "../vf_test.h"

#define VF_SPARSESET_IMPLEMENTATION
#include "../vf_sparseset.h"

#include <stdbool.h>
#include <stdio.h>

TEST(SparseSet, Create) {
    size_t capacity = 8;

    // Create a sparse set
    vf_sparse_set_t* set = vf_sparse_set_create(capacity, sizeof(int));
    EXPECT_NE(set, NULL);

    // Check if set is empty as it should be
    EXPECT_EQ(set->size, 0);

    // Check if capacity is `capacity`
    EXPECT_EQ(set->capacity, capacity);

    vf_sparse_set_destroy(set);
    return true;
}

TEST(SparseSet, Insert) {
    vf_sparse_set_t* set = vf_sparse_set_create(4, sizeof(int));

    // Test insert
    int data1 = 21;
    EXPECT_TRUE(vf_sparse_set_insert(set, 1, &data1));
    // See if size is ok
    EXPECT_EQ(set->size, 1);
    // See if the inserted data is correct
    EXPECT_EQ(*(int*)vf_sparse_set_get(set, 1), data1);

    // Insert more data
    int data2 = 9;
    int data3 = 15;
    EXPECT_TRUE(vf_sparse_set_insert(set, 2, &data2));
    EXPECT_TRUE(vf_sparse_set_insert(set, 3, &data3));
    EXPECT_EQ(set->size, 3);

    // Try to insert existing ID
    EXPECT_FALSE(vf_sparse_set_insert(set, 1, &data2));
    EXPECT_EQ(set->size, 3);

    // Insert beyond initial capacity
    int data4 = 42;
    EXPECT_TRUE(vf_sparse_set_insert(set, 10, &data4));
    EXPECT_EQ(set->size, 4);
    EXPECT_GT(set->sparse_capacity, 10);

    vf_sparse_set_destroy(set);
    return true;
}

TEST(SparseSet, Get) {
    vf_sparse_set_t* set = vf_sparse_set_create(4, sizeof(int));

    int data1 = 21, data2 = 9;
    vf_sparse_set_insert(set, 1, &data1);
    vf_sparse_set_insert(set, 5, &data2);

    // Test get
    EXPECT_EQ(*(int*)vf_sparse_set_get(set, 1), data1);
    EXPECT_EQ(*(int*)vf_sparse_set_get(set, 5), data2);

    // Test get for non-existent ID
    EXPECT_EQ(vf_sparse_set_get(set, 2), NULL);

    // Test get for out-of-range ID
    EXPECT_EQ(vf_sparse_set_get(set, 10), NULL);

    vf_sparse_set_destroy(set);
    return true;
}

TEST(SparseSet, Has) {
    vf_sparse_set_t* set = vf_sparse_set_create(4, sizeof(int));

    int data = 21;
    vf_sparse_set_insert(set, 1, &data);

    // Test has
    EXPECT_TRUE(vf_sparse_set_has(set, 1));
    EXPECT_FALSE(vf_sparse_set_has(set, 2));
    EXPECT_FALSE(vf_sparse_set_has(set, 10));  // Out of range

    vf_sparse_set_destroy(set);
    return true;
}

TEST(SparseSet, Set) {
    vf_sparse_set_t* set = vf_sparse_set_create(4, sizeof(int));

    int data1 = 21, data2 = 9;
    vf_sparse_set_insert(set, 1, &data1);

    // Test set
    EXPECT_TRUE(vf_sparse_set_set(set, 1, &data2));
    EXPECT_EQ(*(int*)vf_sparse_set_get(set, 1), data2);

    // Test set for non-existent ID
    EXPECT_FALSE(vf_sparse_set_set(set, 2, &data1));

    vf_sparse_set_destroy(set);
    return true;
}

TEST(SparseSet, Remove) {
    vf_sparse_set_t* set = vf_sparse_set_create(4, sizeof(int));

    int data1 = 21, data2 = 9, data3 = 15;
    vf_sparse_set_insert(set, 1, &data1);
    vf_sparse_set_insert(set, 2, &data2);
    vf_sparse_set_insert(set, 3, &data3);

    // Test remove
    vf_sparse_set_remove(set, 2);
    EXPECT_EQ(set->size, 2);
    EXPECT_FALSE(vf_sparse_set_has(set, 2));

    // Check if the last element was moved to the removed position
    EXPECT_EQ(*(int*)vf_sparse_set_get(set, 3), data3);

    // Test remove non-existent ID (should not crash)
    vf_sparse_set_remove(set, 10);
    EXPECT_EQ(set->size, 2);

    vf_sparse_set_destroy(set);
    return true;
}

TEST(SparseSet, Iterator) {
    vf_sparse_set_t* set = vf_sparse_set_create(4, sizeof(int));

    int data1 = 21, data2 = 9, data3 = 15;
    vf_sparse_set_insert(set, 5, &data1);
    vf_sparse_set_insert(set, 2, &data2);
    vf_sparse_set_insert(set, 8, &data3);

    // Test iterator
    vf_sparse_set_iterator_t it = vf_sparse_set_iterator(set);
    uint32_t id;
    void* data;
    int count = 0;
    while (vf_sparse_set_iterator_next(&it, &id, &data)) {
        count++;
        EXPECT_TRUE(id == 5 || id == 2 || id == 8);
        int value = *(int*)data;
        EXPECT_TRUE(value == data1 || value == data2 || value == data3);
    }
    EXPECT_EQ(count, 3);

    vf_sparse_set_destroy(set);
    return true;
}

TEST(SparseSet, LargeDataType) {
    typedef struct {
        double x, y, z;
        int id;
        char name[20];
    } LargeData;

    vf_sparse_set_t* set = vf_sparse_set_create(4, sizeof(LargeData));

    LargeData data1 = {1.0, 2.0, 3.0, 1, "Object 1"};
    LargeData data2 = {4.0, 5.0, 6.0, 2, "Object 2"};

    EXPECT_TRUE(vf_sparse_set_insert(set, 1, &data1));
    EXPECT_TRUE(vf_sparse_set_insert(set, 2, &data2));

    LargeData* retrieved = (LargeData*)vf_sparse_set_get(set, 1);
    EXPECT_EQ(retrieved->x, data1.x);
    EXPECT_EQ(retrieved->y, data1.y);
    EXPECT_EQ(retrieved->z, data1.z);
    EXPECT_EQ(retrieved->id, data1.id);
    // EXPECT_STREQ(retrieved->name, data1.name);

    vf_sparse_set_destroy(set);
    return true;
}
