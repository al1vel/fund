#include <gtest/gtest.h>
#include "my_lib.h"

class TestFoo : public ::testing::Test {
};

TEST_F(TestFoo, test_add)
{
    Array<int> a(10);
    ASSERT_EQ(a.size(), 10);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}