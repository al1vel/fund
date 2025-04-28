#include "vector.h"
#include <gtest/gtest.h>

TEST(VectorTest, DefaultConstructor) {
    cont::Vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
}

TEST(VectorTest, FillConstructor) {
    cont::Vector<int> v(5, 42);
    EXPECT_EQ(v.size(), 5);
    EXPECT_GE(v.capacity(), 5);
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(v[i], 42);
    }
}

TEST(VectorTest, InitializerListConstructor) {
    cont::Vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 4);
}

TEST(VectorTest, CopyConstructor) {
    cont::Vector<int> v1 = {5, 6, 7};
    cont::Vector<int> v2 = v1;
    EXPECT_EQ(v1, v2);
}

TEST(VectorTest, MoveConstructor) {
    cont::Vector<int> v1 = {8, 9};
    cont::Vector<int> v2 = std::move(v1);
    EXPECT_EQ(v2.size(), 2);
    EXPECT_EQ(v2[0], 8);
    EXPECT_EQ(v2[1], 9);
}

TEST(VectorTest, CopyAssignment) {
    cont::Vector<int> v1 = {10, 11, 12};
    cont::Vector<int> v2;
    v2 = v1;
    EXPECT_EQ(v1, v2);
}

TEST(VectorTest, CopyAssignment2) {
    cont::Vector<int> v1 = {10, 11, 12};
    cont::Vector<int> v2 = {1, 2, 3, 4};
    v2 = v1;
    EXPECT_EQ(v1, v2);
}

TEST(VectorTest, MoveAssignment) {
    cont::Vector<int> v1 = {13, 14};
    cont::Vector<int> v2;
    v2 = std::move(v1);
    EXPECT_EQ(v2.size(), 2);
    EXPECT_EQ(v2[0], 13);
    EXPECT_EQ(v2[1], 14);
}

TEST(VectorTest, PushBackAndPopBack) {
    cont::Vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.back(), 3);

    v.pop_back();
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.back(), 2);
}

TEST(VectorTest, PopExc) {
    cont::Vector<int> v;
    EXPECT_THROW(v.pop_back(), std::out_of_range);
}

TEST(VectorTest, Insert) {
    cont::Vector<int> v = {1, 2, 4};
    v.insert(2, 3); // Insert 3 at index 2
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 4);
}

TEST(VectorTest, InsertEmpty) {
    cont::Vector<int> v;
    v.insert(2, 10); // Insert 3 at index 2
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[2], 10);
}

TEST(VectorTest, Erase) {
    cont::Vector<int> v = {1, 2, 3, 4};
    v.erase(1); // erase element 2
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 3);
    EXPECT_EQ(v[2], 4);
    EXPECT_THROW(v.erase(4), std::out_of_range);
}

TEST(VectorTest, Clear) {
    cont::Vector<int> v = {1, 2, 3};
    v.clear();
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_THROW(v.clear(), std::out_of_range);
}

TEST(VectorTest, ReserveAndCapacity) {
    cont::Vector<int> v;
    v.reserve(100);
    EXPECT_GE(v.capacity(), 100);
}

TEST(VectorTest, ShrinkToFit) {
    cont::Vector<int> v;
    v.reserve(100);
    v.push_back(1);
    v.push_back(2);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), v.size());
}

TEST(VectorTest, ResizeGrow) {
    cont::Vector<int> v = {1, 2};
    v.resize(5);
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 0);
    EXPECT_EQ(v[3], 0);
    EXPECT_EQ(v[4], 0);
}

TEST(VectorTest, ResizeShrink) {
    cont::Vector<int> v = {1, 2, 3, 4, 5};
    v.resize(2);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
}

TEST(VectorTest, FrontAndBack) {
    cont::Vector<int> v = {5, 6, 7};
    EXPECT_EQ(v.front(), 5);
    EXPECT_EQ(v.back(), 7);
}

TEST(VectorTest, ComparisonOperatorsEqual) {
    cont::Vector<int> v1 = {1, 2, 3};
    cont::Vector<int> v2 = {1, 2, 3};
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 != v2);
}

TEST(VectorTest, ComparisonOperatorsNotEqual) {
    cont::Vector<int> v1 = {1, 2, 3};
    cont::Vector<int> v2 = {1, 2, 4};
    EXPECT_TRUE(v1 != v2);
    EXPECT_FALSE(v1 == v2);
}

TEST(VectorTest, SpaceshipOperatorLess) {
    cont::Vector<int> v1 = {1, 2, 3};
    cont::Vector<int> v2 = {1, 2, 4};
    EXPECT_TRUE((v1 < v2));
}

TEST(VectorTest, SpaceshipOperatorGreater) {
    cont::Vector<int> v1 = {2, 2, 3};
    cont::Vector<int> v2 = {1, 2, 4};
    EXPECT_TRUE((v1 > v2));
}

TEST(VectorTest, SpaceshipOperatorEqualBySize) {
    cont::Vector<int> v1 = {1, 2};
    cont::Vector<int> v2 = {1, 2, 3};
    EXPECT_TRUE((v1 < v2));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}