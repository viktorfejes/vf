#define VF_TEST_IMPLEMENTATION
#include "../vf_test.h"

#include "test_vf_darray.h"
#include "test_vf_queue.h"
#include "test_vf_hashmap.h"
#include "test_vf_binaryheap.h"
#include "test_vf_sparseset.h"
// #include "test_vf_memory_pool.h"
#include "test_vf_thread.h"

int main(void) {
    RUN_ALL_TESTS();
    return 0;
}
