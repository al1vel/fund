#include <gtest/gtest.h>
#include "my_lib.h"

class TestFoo : public ::testing::Test {
protected:
    cont::Array<int, 10> ar;

    void SetUp() override {
        for (int i = 0; i < 10; i++) {
            ar[i] = i;
        }
    }

    void TearDown() override {

    }
};

TEST_F(TestFoo, test_size) {
    ASSERT_EQ(ar.size(), 10);
}

TEST_F(TestFoo, test_max_size) {
    ASSERT_EQ(ar.max_size(), 10);
}

TEST_F(TestFoo, test_at_good) {
    ASSERT_EQ(ar.at(2), 2);
}

TEST_F(TestFoo, test_at_bad) {
    ASSERT_THROW(ar.at(10), std::out_of_range);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}