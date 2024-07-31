/*
*   vf_hashmap - v0.1
*   Header-only tiny hashmap library using 64-bit FNV-1a hash
*   and open addressing with linear probing to handle collisions.
*
*   RECENT CHANGES:
*       0.1     (2024-07-30)    Finalized the implementation;
*
*   LICENSE: MIT License
*       Copyright (c) 2024 Viktor Fejes
*
*       Permission is hereby granted, free of charge, to any person obtaining a copy
*       of this software and associated documentation files (the "Software"), to deal
*       in the Software without restriction, including without limitation the rights
*       to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*       copies of the Software, and to permit persons to whom the Software is
*       furnished to do so, subject to the following conditions:
*
*       The above copyright notice and this permission notice shall be included in all
*       copies or substantial portions of the Software.
*
*       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*       IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*       FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*       AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*       LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*       OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*       SOFTWARE.
*
*   TODOs:
*       - [ ] Add preprocessor to be able to set the multiplication
*             factor when resizing hash maps.
*
 */

#ifndef VF_HASHMAP_H
#define VF_HASHMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define VF_HASH_INITIAL_CAPACITY 16
#define VF_HASH_LOAD_FACTOR 0.75

typedef struct {
    char* key;
    void* value;
} vf_hashmap_entry_t;

typedef struct {
    vf_hashmap_entry_t* entries;
    size_t capacity;
    size_t size;
} vf_hashmap_t;

extern vf_hashmap_t* vf_hashmap_create(void);
extern void vf_hashmap_free(vf_hashmap_t* map);
extern int vf_hashmap_insert(vf_hashmap_t* map, const char* key, void* value);
extern void* vf_hashmap_get(vf_hashmap_t* map, const char* key);
extern void vf_hashmap_erase(vf_hashmap_t* map, const char* key);
extern size_t vf_hashmap_size(vf_hashmap_t* map);
extern size_t vf_hashmap_capacity(vf_hashmap_t* map);

#ifdef __cplusplus
}
#endif

// END OF HEADER. -----------------------------------------

#ifdef VF_HASHMAP_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>

// FNV-1a 64-bit hash function
static uint64_t _hash_key(const char* key) {
    uint64_t hash = 14695981039346656037ULL;    // FNV offset (64-bit)
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= 1099511628211ULL;               // FNV prime
    }
    return hash;
}

static int _hashmap_set_entry(vf_hashmap_entry_t* entries, size_t capacity, const char* key, void* value, size_t* plength) {
    uint64_t hash = _hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            entries[index].value = value;
            return 0;
        }
        index++;
        if (index >= capacity) index = 0;
    }

    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) return -1;
        (*plength)++;
    }
    entries[index].key = (char*)key;
    entries[index].value = value;
    return 1;
}

static int _hashmap_expand(vf_hashmap_t* map) {
    // Double the capacity
    // TODO: this could be defined as multiplication number
    size_t new_capacity = map->capacity * 2;

    vf_hashmap_entry_t* new_entries = (vf_hashmap_entry_t*)calloc(new_capacity, sizeof(vf_hashmap_entry_t));
    if (!new_entries) return -1;

    for (size_t i = 0; i < map->capacity; ++i) {
        vf_hashmap_entry_t entry = map->entries[i];
        if (entry.key != NULL) {
            _hashmap_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
        }
    }

    free(map->entries);
    map->entries = new_entries;
    map->capacity = new_capacity;

    return 0;
}

vf_hashmap_t* vf_hashmap_create(void) {
    vf_hashmap_t* map = (vf_hashmap_t*)malloc(sizeof(vf_hashmap_t));
    if (!map) return NULL;

    map->entries = (vf_hashmap_entry_t*)calloc(VF_HASH_INITIAL_CAPACITY, sizeof(vf_hashmap_entry_t));
    if (!map->entries) {
        free(map);
        return NULL;
    }

    map->capacity = VF_HASH_INITIAL_CAPACITY;
    map->size = 0;
    return map;
}

int vf_hashmap_insert(vf_hashmap_t* map, const char* key, void* value) {
    if (map->size >= map->capacity * VF_HASH_LOAD_FACTOR) {
        if (_hashmap_expand(map) == -1) return -1;
    }

    int status = _hashmap_set_entry(map->entries, map->capacity, key, value, &map->size);
    if (status == -1) return -1;

    return 0;
}

void* vf_hashmap_get(vf_hashmap_t* map, const char* key) {
    uint64_t hash = _hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

    while (map->entries[index].key != NULL) {
        if (strcmp(key, map->entries[index].key) == 0) {
            return map->entries[index].value;
        }
        index++;
        if (index >= map->capacity) index = 0;
    }

    return NULL;
}

void vf_hashmap_erase(vf_hashmap_t* map, const char* key) {
    uint64_t hash = _hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

    while (map->entries[index].key != NULL) {
        if (strcmp(key, map->entries[index].key) == 0) {
            free(map->entries[index].key);
            map->entries[index].key = NULL;
            map->entries[index].value = NULL;
            map->size--;
            return;
        }
        index++;
        if (index >= map->capacity) index = 0;
    }
}

void vf_hashmap_free(vf_hashmap_t* map) {
    for (size_t i = 0; i < map->capacity; ++i) {
        free(map->entries[i].key);
    }
    free(map->entries);
    free(map);
}

size_t vf_hashmap_size(vf_hashmap_t* map) {
    return map->size;
}

size_t vf_hashmap_capacity(vf_hashmap_t* map) {
    return map->capacity;
}

#endif // VF_HASHMAP_IMPLEMENTATION
#endif // VF_HASHMAP_H
