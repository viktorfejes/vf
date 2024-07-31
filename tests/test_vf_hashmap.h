#include "../vf_test.h"

#define VF_HASHMAP_IMPLEMENTATION
#include "../vf_hashmap.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

TEST(Hashmap, Create) {
    // Create a hash map
    vf_hashmap_t* map = vf_hashmap_create();
    EXPECT_NE(map, NULL);

    // Check if hash map is empty as it should be
    EXPECT_EQ(vf_hashmap_size(map), 0);

    // Check if capacity is INITIAL_CAPACITY (assuming it's 16)
    EXPECT_EQ(vf_hashmap_capacity(map), 16);

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, SetAndGet) {
    vf_hashmap_t* map = vf_hashmap_create();

    // Test set and get
    const char* key1 = "key1";
    int value1 = 42;
    EXPECT_EQ(vf_hashmap_insert(map, key1, &value1), 0);
    EXPECT_EQ(vf_hashmap_size(map), 1);
    EXPECT_EQ(*(int*)vf_hashmap_get(map, key1), value1);

    // Test overwriting existing key
    int value2 = 24;
    EXPECT_EQ(vf_hashmap_insert(map, key1, &value2), 0);
    EXPECT_EQ(vf_hashmap_size(map), 1);
    EXPECT_EQ(*(int*)vf_hashmap_get(map, key1), value2);

    // Test multiple entries
    const char* key2 = "key2";
    const char* key3 = "key3";
    int value3 = 15;
    EXPECT_EQ(vf_hashmap_insert(map, key2, &value1), 0);
    EXPECT_EQ(vf_hashmap_insert(map, key3, &value3), 0);
    EXPECT_EQ(vf_hashmap_size(map), 3);
    EXPECT_EQ(*(int*)vf_hashmap_get(map, key2), value1);
    EXPECT_EQ(*(int*)vf_hashmap_get(map, key3), value3);

    // Test get non-existent key
    EXPECT_EQ(vf_hashmap_get(map, "non_existent"), NULL);

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, Delete) {
    vf_hashmap_t* map = vf_hashmap_create();

    const char* key1 = "key1";
    const char* key2 = "key2";
    int value1 = 42;
    int value2 = 24;

    vf_hashmap_insert(map, key1, &value1);
    vf_hashmap_insert(map, key2, &value2);
    EXPECT_EQ(vf_hashmap_size(map), 2);

    // Delete existing key
    vf_hashmap_erase(map, key1);
    EXPECT_EQ(vf_hashmap_size(map), 1);
    EXPECT_EQ(vf_hashmap_get(map, key1), NULL);
    EXPECT_EQ(*(int*)vf_hashmap_get(map, key2), value2);

    // Delete non-existent key (should not affect size)
    vf_hashmap_erase(map, "non_existent");
    EXPECT_EQ(vf_hashmap_size(map), 1);

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, Expansion) {
    vf_hashmap_t* map = vf_hashmap_create();
    int value = 42;

    // Add entries until expansion occurs
    for (int i = 0; i < 20; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        EXPECT_EQ(vf_hashmap_insert(map, key, &value), 0);
    }

    // Check if capacity has increased (should be more than initial 16)
    EXPECT_GT(vf_hashmap_capacity(map), 16);

    // Verify all entries are still accessible
    for (int i = 0; i < 20; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        EXPECT_EQ(*(int*)vf_hashmap_get(map, key), value);
    }

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, StringKeys) {
    vf_hashmap_t* map = vf_hashmap_create();

    const char* keys[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int values[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(vf_hashmap_insert(map, keys[i], &values[i]), 0);
    }

    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(*(int*)vf_hashmap_get(map, keys[i]), values[i]);
    }

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, Collisions) {
    vf_hashmap_t* map = vf_hashmap_create();

    // These keys should collide in most hash implementations
    const char* key1 = "abcdef";
    const char* key2 = "abcfed";
    int value1 = 111;
    int value2 = 222;

    EXPECT_EQ(vf_hashmap_insert(map, key1, &value1), 0);
    EXPECT_EQ(vf_hashmap_insert(map, key2, &value2), 0);

    EXPECT_EQ(*(int*)vf_hashmap_get(map, key1), value1);
    EXPECT_EQ(*(int*)vf_hashmap_get(map, key2), value2);

    vf_hashmap_free(map);
    return true;
}
