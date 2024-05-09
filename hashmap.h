#pragma once

#include "defines.h"
#include "mem.h"

#include <stdlib.h>

// TODO: temp!
#include <string.h>

// NOTE: Handling collisions with chaining
typedef struct hashmap_bucket {
    char* key;
    void* value;
    hashmap_bucket* next;
} hashmap_bucket;

typedef struct hashmap {
    u8 prime_index;
    u64 bucket_count;
    u64 count;
    hashmap_bucket* buckets;
} hashmap;

// At what capacity should resize triggered.
#define HM_FILL_CAP 0.75

// The possible bucket counts
// NOTE: Using approx. doubling prime numbers for favourable hash distribution
#define HM_PRIME_LIST                                                                              \
    5, 13, 29, 61, 127, 263, 431, 863, 1723, 3449, 7001, 14009, 28019, 56081, 112163, 224891,      \
        498557, 998071, 1801517, 3601579, 7201751, 14403443, 28803469, 56603333, 122202757,        \
        244501553, 484500161, 961507429, 1861507757, 2147483647

static const u64 PRIME_TABLE[] = { HM_PRIME_LIST };
static const u64 NUM_PRIMES = sizeof(PRIME_TABLE) / sizeof(PRIME_TABLE[0]);

// Hash defines
#define DJB2_PRIME       5381
#define FNV_PRIME        16777619
#define FNV_OFFSET_BASIS 2166136261U

/**
 * @brief Hashing function using DJB2 hash.
 * 
 * @param str Null terminated string.
 * @return u32 Hashed string.
 * @note http://www.cse.yorku.ca/~oz/hash.html
 */
INLINE u32 djb2(const char* str) {
    u32 hash = DJB2_PRIME;
    i32 c;

    while ((c = *str)) { hash = ((hash << 5) + hash) + c; }

    return hash;
}

/**
 * @brief Hash function using FNV-1a hash.
 * 
 * @param str Null terminated string.
 * @return u32 Hashed string.
 * @note https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
INLINE u32 fnv1a(const char* str) {
    u32 hash = FNV_OFFSET_BASIS;
    i32 c;

    while ((c = *str++)) {
        hash ^= c;
        hash *= FNV_PRIME;
    }

    return hash;
}

/**
 * @brief Calculates index into the current bucket count Hashmap.
 * 
 * @param hash Hashed key.
 * @param prime_index Index into the prime table to determine size (bucket count).
 * @return u64 Index
 */
INLINE u64 hashmap_calc_index(u32 hash, u8 prime_index) {
    return hash % PRIME_TABLE[prime_index];
}

/**
 * @brief Allocates a new Hashmap.
 * 
 * @return hashmap* A new, empty Hashmap with default capacity (bucket count).
 */
INLINE hashmap* hashmap_alloc() {
    hashmap* hm = (hashmap*)malloc(sizeof(hashmap));
    hm->prime_index = 0;
    hm->bucket_count = PRIME_TABLE[0];
    hm->count = 0;
    hm->buckets = (hashmap_bucket*)malloc(sizeof(hashmap_bucket) * PRIME_TABLE[0]);

    return hm;
}

INLINE hashmap* hashmap_reserve(hashmap* hm, u64 desired_size) {}

/**
 * @brief Inserts an element into the Hashmap.
 * 
 * @param hm Pointer to the Hashmap.
 * @param key Key for the element to be inserted in.
 * @param value Value for the element to be inserted in.
 * @param size Size of the value.
 * @return hashmap* Pointer to the Hashmap.
 */
INLINE hashmap* hashmap_insert(hashmap* hm, const char* key, void* value, u64 size) {
    // If the Hashmap is at least 75% full, we reserve more space
    if ((hm->count / hm->bucket_count) >= HM_FILL_CAP) {
        hashmap_reserve(hm, PRIME_TABLE[hm->prime_index + 1]);
    }

    u32 hash = fnv1a(key);
    u64 index = hashmap_calc_index(hash, hm->prime_index);

    // If the index already exist, and the key is not the same
    // chain it... (this is a collision!)
    // TODO: we need string comparison here!
    if (hm->buckets[index].key && hm->buckets[index].key != key) {
        // Create a new bucket...
        hashmap_bucket* new_bucket = (hashmap_bucket*)malloc(sizeof(hashmap_bucket));
        new_bucket->key = strdup(key); // TODO: own implementation
        new_bucket->value = malloc(size);
        mem_copy(new_bucket->value, value, size);

        // ... and make it the head in the next chain.
        new_bucket->next = hm->buckets[index].next;
        hm->buckets[index].next = new_bucket;
    } else {
        hm->buckets[index].key = strdup(key); // TODO: own implementation
        hm->buckets[index].value = malloc(size);
        mem_copy(hm->buckets[index].value, value, size);
    }

    hm->count++;

    return hm;
}

/**
 * @brief Frees the Hashmap
 * 
 * @param hm Pointer to the Hashmap to be freed.
 */
INLINE void hashmap_free(hashmap* hm) {
    // TODO: Might need to free all the chained buckets...
    // Maybe the bucket freeing could be a separate method that takes care of that.

    free(hm->buckets);
    free(hm);
}