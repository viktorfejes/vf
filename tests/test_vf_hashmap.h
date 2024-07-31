#include "../vf_test.h"

#define VF_HASHMAP_IMPLEMENTATION
#include "../vf_hashmap.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

TEST(Hashmap, Create) {
    // Create a hash map for int values
    vf_hashmap_t* map = vf_hashmap_create(sizeof(int));
    EXPECT_NE(map, NULL);

    // Check if hash map is empty as it should be
    EXPECT_EQ(vf_hashmap_size(map), 0);

    // Check if capacity is VF_HASH_INITIAL_CAPACITY (16)
    EXPECT_EQ(vf_hashmap_capacity(map), VF_HASH_INITIAL_CAPACITY);

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, SetAndGet) {
    vf_hashmap_t* map = vf_hashmap_create(sizeof(int));

    // Test set and get
    const char* key1 = "key1";
    int value1 = 42;
    EXPECT_EQ(vf_hashmap_set(map, key1, &value1), 0);
    EXPECT_EQ(vf_hashmap_size(map), 1);
    EXPECT_EQ(*(const int*)vf_hashmap_get(map, key1), value1);

    // Test overwriting existing key
    int value2 = 24;
    EXPECT_EQ(vf_hashmap_set(map, key1, &value2), 0);
    EXPECT_EQ(vf_hashmap_size(map), 1);
    EXPECT_EQ(*(const int*)vf_hashmap_get(map, key1), value2);

    // Test multiple entries
    const char* key2 = "key2";
    const char* key3 = "key3";
    int value3 = 15;
    EXPECT_EQ(vf_hashmap_set(map, key2, &value1), 0);
    EXPECT_EQ(vf_hashmap_set(map, key3, &value3), 0);
    EXPECT_EQ(vf_hashmap_size(map), 3);
    EXPECT_EQ(*(const int*)vf_hashmap_get(map, key2), value1);
    EXPECT_EQ(*(const int*)vf_hashmap_get(map, key3), value3);

    // Test get non-existent key
    EXPECT_EQ(vf_hashmap_get(map, "non_existent"), NULL);

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, Has) {
    vf_hashmap_t* map = vf_hashmap_create(sizeof(int));

    const char* key1 = "key1";
    const char* key2 = "key2";
    int value = 42;

    // Test on empty map
    EXPECT_EQ(vf_hashmap_has(map, key1), 0);

    // Add a key and test
    vf_hashmap_set(map, key1, &value);
    EXPECT_EQ(vf_hashmap_has(map, key1), 1);
    EXPECT_EQ(vf_hashmap_has(map, key2), 0);

    // Remove key and test again
    vf_hashmap_remove(map, key1);
    EXPECT_EQ(vf_hashmap_has(map, key1), 0);

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, GetMutable) {
    vf_hashmap_t* map = vf_hashmap_create(sizeof(int));

    const char* key = "key";
    int value = 42;
    vf_hashmap_set(map, key, &value);

    // Get mutable pointer and modify value
    int* mutable_value = (int*)vf_hashmap_get_mutable(map, key);
    EXPECT_NE(mutable_value, NULL);
    *mutable_value = 24;

    // Check if the value was actually modified
    EXPECT_EQ(*(const int*)vf_hashmap_get(map, key), 24);

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, Remove) {
    vf_hashmap_t* map = vf_hashmap_create(sizeof(int));

    const char* key1 = "key1";
    const char* key2 = "key2";
    int value1 = 42;
    int value2 = 24;

    vf_hashmap_set(map, key1, &value1);
    vf_hashmap_set(map, key2, &value2);
    EXPECT_EQ(vf_hashmap_size(map), 2);

    // Remove existing key
    vf_hashmap_remove(map, key1);
    EXPECT_EQ(vf_hashmap_size(map), 1);
    EXPECT_EQ(vf_hashmap_get(map, key1), NULL);
    EXPECT_EQ(*(const int*)vf_hashmap_get(map, key2), value2);

    // Remove non-existent key (should not affect size)
    vf_hashmap_remove(map, "non_existent");
    EXPECT_EQ(vf_hashmap_size(map), 1);

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, Expansion) {
    vf_hashmap_t* map = vf_hashmap_create(sizeof(int));
    int value = 42;

    // Add entries until expansion occurs
    for (int i = 0; i < 20; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        EXPECT_EQ(vf_hashmap_set(map, key, &value), 0);
    }

    // Check if capacity has increased (should be more than initial 16)
    EXPECT_GT(vf_hashmap_capacity(map), VF_HASH_INITIAL_CAPACITY);

    // Verify all entries are still accessible
    for (int i = 0; i < 20; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        EXPECT_EQ(*(const int*)vf_hashmap_get(map, key), value);
    }

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, StringKeys) {
    vf_hashmap_t* map = vf_hashmap_create(sizeof(int));

    const char* keys[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int values[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(vf_hashmap_set(map, keys[i], &values[i]), 0);
    }

    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(*(const int*)vf_hashmap_get(map, keys[i]), values[i]);
    }

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, Collisions) {
    vf_hashmap_t* map = vf_hashmap_create(sizeof(int));

    // These keys should collide in most hash implementations
    const char* key1 = "abcdef";
    const char* key2 = "abcfed";
    int value1 = 111;
    int value2 = 222;

    EXPECT_EQ(vf_hashmap_set(map, key1, &value1), 0);
    EXPECT_EQ(vf_hashmap_set(map, key2, &value2), 0);

    EXPECT_EQ(*(const int*)vf_hashmap_get(map, key1), value1);
    EXPECT_EQ(*(const int*)vf_hashmap_get(map, key2), value2);

    vf_hashmap_free(map);
    return true;
}

TEST(Hashmap, DifferentTypes) {
    // Test with float
    vf_hashmap_t* float_map = vf_hashmap_create(sizeof(float));
    const char* float_key = "pi";
    float float_value = 3.14159f;
    EXPECT_EQ(vf_hashmap_set(float_map, float_key, &float_value), 0);
    EXPECT_EQ(*(const float*)vf_hashmap_get(float_map, float_key), float_value);
    vf_hashmap_free(float_map);

    // Test with char
    vf_hashmap_t* char_map = vf_hashmap_create(sizeof(char));
    const char* char_key = "initial";
    char char_value = 'A';
    EXPECT_EQ(vf_hashmap_set(char_map, char_key, &char_value), 0);
    EXPECT_EQ(*(const char*)vf_hashmap_get(char_map, char_key), char_value);
    vf_hashmap_free(char_map);

    return true;
}
