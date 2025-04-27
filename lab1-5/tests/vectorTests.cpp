#include "vector.h"
#include <gtest/gtest.h>

struct HUI {
    int x;
    int y;
};

TEST(Constructors, Default) {
    cont::Vector<HUI*> vec;
    std::cout << "hui" << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}