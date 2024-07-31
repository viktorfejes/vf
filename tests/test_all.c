#define VF_TEST_IMPLEMENTATION
#include "../vf_test.h"

#include "test_vf_darray.h"
#include "test_vf_queue.h"
#include "test_vf_hashmap.h"
#include "test_vf_binaryheap.h"

int main(void) {
    RUN_ALL_TESTS();
    return 0;
}
