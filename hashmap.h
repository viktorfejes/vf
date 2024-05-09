#pragma once

#include "defines.h"

typedef struct hashmap_kv {
    char* key;
    void* value;
} hashmap_kv;

typedef struct hashmap {
    hashmap_kv** elements;
} hashmap;