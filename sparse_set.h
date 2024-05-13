#pragma once

#include "darray.h"
#include "defines.h"

typedef struct sparse_set {
    u64* sparse;
    u64* dense;
} sparse_set;

INLINE void sparse_set_init(sparse_set* ss) {
    ss->sparse = (u64*)da_alloc(sizeof(u64));
    ss->dense = (u64*)da_alloc(sizeof(u64));
}

INLINE void sparse_set_free(sparse_set* ss) {
    da_free(ss->sparse);
    da_free(ss->dense);
}