#define VF_TEST_IMPLEMENTATION
#include "../vf_test.h"

TEST(Example, Name) {
    EXPECT_TRUE(1 == 1);
    EXPECT_FALSE(1 == 2);
    return true;
}

TEST(Another, TestName) {
    EXPECT_TRUE(1 == 2);
    return true;
}

TEST(CreationLogic, Create) {
    EXPECT_EQ(1, 2);
    return true;
}

int main(void) {
    RUN_ALL_TESTS();
    return 0;
}
