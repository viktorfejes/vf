#include "mem_test.h"
#include "../test.h"
#include "../test_manager.h"

#include "../../../mem.h"

unsigned char mem_set_test() {
    char str[] = "almost every programmer should know memset!";
    mem_set(str, '-', 6);
    printf("%s\n", str);

    return true;
}

void mem_register_tests() {
    test_manager_register_test(mem_set_test, "Mem - Set");
}