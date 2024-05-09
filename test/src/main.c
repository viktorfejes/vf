#include "test_manager.h"

#include "darray/darray_test.h"
#include "mem/mem_test.h"
#include "thread/thread_test.h"

int main(void) {
    test_manager_init();

    // REGISTER TESTS
    darray_register_tests();
    mem_register_tests();
    thread_register_tests();
    // REGISTER TESTS END

    test_manager_run_tests();

    return 0;
}