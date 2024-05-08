#include "darray_test.h"
#include "../test.h"
#include "../test_manager.h"

#include "../../../darray.h"

unsigned char darray_create_and_free() {
    // Test creation
    int* my_darray = da_alloc(sizeof(int));
    REQUIRE(da_is_empty(my_darray) == true);
    REQUIRE(da_count(my_darray) == 0);
    REQUIRE(da_capacity(my_darray) == DA_DEFAULT_CAPACITY);
    REQUIRE(da_stride(my_darray) == sizeof(int));

    // Test free
    da_free(my_darray);
    REQUIRE(da_capacity(my_darray) == 0);
    REQUIRE(_da_header_get(my_darray, DA_MAX_CAPACITY) == 0);
    REQUIRE(da_stride(my_darray) == 0);

    return true;
}

unsigned char darray_alloc_exact_and_reserve() {
    // Creating exact size darray
    int* my_darray = da_alloc_exact(16, sizeof(int));
    REQUIRE(da_is_empty(my_darray) == true);
    REQUIRE(da_count(my_darray) == 0);
    REQUIRE(da_capacity(my_darray) == 16);
    REQUIRE(da_stride(my_darray) == sizeof(int));

    // Reserve less
    my_darray = da_reserve(my_darray, 8);
    REQUIRE(da_is_empty(my_darray) == true);
    REQUIRE(da_count(my_darray) == 0);
    REQUIRE(da_capacity(my_darray) == 16);
    REQUIRE(da_stride(my_darray) == sizeof(int));

    // Reserve more
    my_darray = da_reserve(my_darray, 32);
    REQUIRE(da_is_empty(my_darray) == true);
    REQUIRE(da_count(my_darray) == 0);
    REQUIRE(da_capacity(my_darray) == 32);
    REQUIRE(da_stride(my_darray) == sizeof(int));

    da_free(my_darray);

    return true;
}

unsigned char darray_resize() {
    // Creating exact size darray
    int* my_darray = da_alloc_exact(16, sizeof(int));

    // Resizing to be smaller
    my_darray = da_resize(my_darray, 8, 0);
    REQUIRE(da_is_empty(my_darray) == true);
    REQUIRE(da_count(my_darray) == 0);
    REQUIRE(da_capacity(my_darray) == 8);
    REQUIRE(da_stride(my_darray) == sizeof(int));

    // Resizing to be bigger
    my_darray = da_resize(my_darray, 16, 0);
    REQUIRE(da_is_empty(my_darray) == true);
    REQUIRE(da_count(my_darray) == 0);
    REQUIRE(da_capacity(my_darray) == 16);
    REQUIRE(da_stride(my_darray) == sizeof(int));

    // Test value fill in
    REQUIRE_TO_BE(0, my_darray[10]);
    REQUIRE_TO_BE(0, my_darray[15]);

    da_free(my_darray);

    return true;
}

unsigned char darray_push_and_insert_and_swap() {
    // Creating exact size darray
    int* my_darray = da_alloc_exact(16, sizeof(int));

    my_darray = da_push_back(my_darray, 42);
    my_darray = da_push_back(my_darray, 16);
    my_darray = da_push_back(my_darray, 32);
    REQUIRE_TO_BE(3, da_count(my_darray));
    REQUIRE_TO_BE(42, my_darray[0]);
    REQUIRE_TO_BE(16, my_darray[1]);
    REQUIRE_TO_BE(32, my_darray[2]);

    // TODO: insert test

    da_swap(my_darray, 0, 2);
    REQUIRE_TO_BE(32, my_darray[0]);
    REQUIRE_TO_BE(42, my_darray[2]);

    return true;
}

void darray_register_tests() {
    test_manager_register_test(darray_create_and_free, "DArray - Create and Free");
    test_manager_register_test(darray_alloc_exact_and_reserve, "DArray - Alloc_exact and Reserve");
    test_manager_register_test(darray_resize, "DArray - Resize");
    test_manager_register_test(darray_push_and_insert_and_swap,
                               "DArray - Push back and Insert and Swap");
}