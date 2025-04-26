#include "stack.h"
#include <gtest/gtest.h>

class DqTest : public ::testing::Test {
protected:
    //contDQ::Deque<int> deque = {1, 2, 3, 4, 5};

    // void SetUp() override {
    //     list.push_back(1);
    //     list.push_back(2);
    //     list.push_back(3);
    // }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}