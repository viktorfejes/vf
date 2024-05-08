#pragma once

typedef unsigned char (*PFN_test)();

#define true  1
#define false 0

void test_manager_init();
void test_manager_register_test(PFN_test, char* desc);
void test_manager_run_tests();