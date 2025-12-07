#define VF_TEST_IMPLEMENTATION
#include "../vf_test.h"

/* ------------------------------------------
   Integer checks
   ------------------------------------------ */

VF_TEST(ints, eq_int_pass) {
    VF_EXPECT_EQ_INT(5, 5);
    VF_ASSERT_EQ_INT(123, 123);
}

VF_TEST(ints, eq_int_fail) {
    VF_EXPECT_EQ_INT(1, 2); /* should fail but continue */
}

VF_TEST(ints, ne_int_pass) {
    VF_EXPECT_NE_INT(1, 2);
    VF_ASSERT_NE_INT(11, 22);
}

VF_TEST(ints, ne_int_fail) {
    VF_EXPECT_NE_INT(7, 7); /* fail */
}

/* ------------------------------------------
   Float checks
   ------------------------------------------ */

VF_TEST(floats, eq_float_pass) {
    VF_EXPECT_EQ_FLOAT(1.0f, 1.0f);
    VF_ASSERT_EQ_FLOAT(2.5f, 2.5f);
}

VF_TEST(floats, eq_float_fail) {
    VF_EXPECT_EQ_FLOAT(1.0f, 2.0f);
}

VF_TEST(floats, near_pass) {
    VF_EXPECT_NEAR_FLOAT(1.0f, 1.001f, 0.01f);
}

VF_TEST(floats, near_fail) {
    VF_EXPECT_NEAR_FLOAT(1.0f, 1.2f, 0.01f);
}

/* ------------------------------------------
   String checks
   ------------------------------------------ */

VF_TEST(strings, eq_str_pass) {
    VF_EXPECT_EQ_STR("hello", "hello");
}

VF_TEST(strings, eq_str_fail) {
    VF_EXPECT_EQ_STR("a", "b");
}

/* ------------------------------------------
   Bool checks
   ------------------------------------------ */

VF_TEST(bools, true_pass) {
    VF_EXPECT_TRUE(1);
    VF_ASSERT_TRUE(42);
}

VF_TEST(bools, true_fail) {
    VF_EXPECT_TRUE(0);
}

VF_TEST(bools, false_pass) {
    VF_EXPECT_FALSE(0);
    VF_ASSERT_FALSE(0);
}

VF_TEST(bools, false_fail) {
    VF_EXPECT_FALSE(5);
}

/* ------------------------------------------
   NULL checks
   ------------------------------------------ */

VF_TEST(nulls, null_pass) {
    void *p = NULL;
    VF_EXPECT_NULL(p);
}

VF_TEST(nulls, null_fail) {
    int x = 1;
    VF_EXPECT_NULL(&x);
}

VF_TEST(nulls, not_null_pass) {
    int x = 5;
    VF_EXPECT_NOT_NULL(&x);
}

VF_TEST(nulls, not_null_fail) {
    void *p = NULL;
    VF_EXPECT_NOT_NULL(p);
}

/* ------------------------------------------
   Inequalities checks
   ------------------------------------------ */

VF_TEST(ineq, gt_pass) {
    VF_EXPECT_GT(5, 4);
}

VF_TEST(ineq, gt_fail) {
    VF_EXPECT_GT(4, 5);
}

VF_TEST(ineq, ge_pass) {
    VF_EXPECT_GE(5, 5);
    VF_EXPECT_GE(6, 5);
}

VF_TEST(ineq, lt_pass) {
    VF_EXPECT_LT(3, 4);
}

VF_TEST(ineq, lt_fail) {
    VF_EXPECT_LT(4, 3);
}

/* ------------------------------------------
   Range checks
   ------------------------------------------ */

VF_TEST(range, in_range_pass) {
    VF_EXPECT_IN_RANGE(5, 0, 10);
}

VF_TEST(range, in_range_fail) {
    VF_EXPECT_IN_RANGE(20, 0, 10);
}

/* ------------------------------------------
   Pointer equality checks
   ------------------------------------------ */

VF_TEST(ptrs, eq_ptr_pass) {
    int  x;
    int *p = &x;
    VF_EXPECT_EQ_PTR(p, &x);
}

VF_TEST(ptrs, eq_ptr_fail) {
    int a, b;
    VF_EXPECT_EQ_PTR(&a, &b);
}

/* ------------------------------------------
   Alignment checks
   ------------------------------------------ */

VF_TEST(align, aligned_pass) {
    int x;
    VF_EXPECT_PTR_ALIGNED(&x, 4);
}

VF_TEST(align, aligned_fail) {
    /* Force misalignment by offsetting 1 byte */
    char buf[16];
    VF_EXPECT_PTR_ALIGNED(buf + 1, 4);
}

/* ------------------------------------------
   Memory compare checks
   ------------------------------------------ */

VF_TEST(mem, memeq_pass) {
    unsigned char a[4] = {1, 2, 3, 4};
    unsigned char b[4] = {1, 2, 3, 4};
    VF_EXPECT_MEMEQ(a, b, 4);
}

VF_TEST(mem, memeq_fail) {
    unsigned char a[4] = {1, 2, 3, 4};
    unsigned char b[4] = {1, 2, 3, 5};
    VF_EXPECT_MEMEQ(a, b, 4);
}

/* ------------------------------------------
   Memory compare checks
   ------------------------------------------ */

VF_TEST(memzero, memzero_pass) {
    unsigned char z[5] = {0, 0, 0, 0, 0};
    VF_EXPECT_MEMZERO(z, 5);
}

VF_TEST(memzero, memzero_fail) {
    unsigned char z[5] = {0, 0, 1, 0, 0};
    VF_EXPECT_MEMZERO(z, 5);
}

/* ------------------------------------------
   HEX32 checks
   ------------------------------------------ */

VF_TEST(hex, hex_pass) {
    VF_EXPECT_EQ_HEX32(0xFF00CC44, 0xFF00CC44);
}

VF_TEST(hex, hex_fail) {
    VF_EXPECT_EQ_HEX32(0xFF00CC44, 0xFF00CC40);
}

/* ------------------------------------------
   Skip behaviour
   ------------------------------------------ */

VF_TEST(skip, skip_this_test) {
    VF_SKIP("Skipping intentionally");
    /* should not run */
    VF_ASSERT_TRUE(0);
}

/* ------------------------------------------
   Additional control
   ------------------------------------------ */

VF_TEST(control, assert_stops) {
    VF_ASSERT_TRUE(0); /* stops here */
    VF_EXPECT_TRUE(1); /* should NEVER run */
}

int main(int argc, char **argv) {
    return vf_test_run(argc, argv);
}
