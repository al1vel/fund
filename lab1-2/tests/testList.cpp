#include "list.h"
#include <gtest/gtest.h>

//using namespace cont;

class ListTest : public ::testing::Test {
protected:
    cont::List<int> list;

    void SetUp() override {
        list.push_back(1);
        list.push_back(2);
        list.push_back(3);
    }
};

TEST_F(ListTest, FrontAndBack) {
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 3);
}

TEST_F(ListTest, PushFront) {
    list.push_front(0);
    EXPECT_EQ(list.front(), 0);
    EXPECT_EQ(list.Size(), 4);
}

TEST_F(ListTest, PopFront) {
    list.pop_front();
    EXPECT_EQ(list.front(), 2);
    EXPECT_EQ(list.Size(), 2);
}

TEST_F(ListTest, PushBack) {
    list.push_back(4);
    EXPECT_EQ(list.back(), 4);
    EXPECT_EQ(list.Size(), 4);
}

TEST_F(ListTest, PopBack) {
    list.pop_back();
    EXPECT_EQ(list.back(), 2);
    EXPECT_EQ(list.Size(), 2);
}

TEST_F(ListTest, Insert) {
    auto it = list.cbegin().next(); // указывает на 2
    list.insert(it, 42);
    auto itCheck = list.begin();
    ++itCheck;
    EXPECT_EQ(*itCheck, 42);
    EXPECT_EQ(list.Size(), 4);
}

TEST_F(ListTest, Erase) {
    auto it = list.cbegin().next(); // указывает на 2
    list.erase(it);
    auto itCheck = ++list.begin();
    EXPECT_EQ(*itCheck, 3);
    EXPECT_EQ(list.Size(), 2);
}

TEST_F(ListTest, ResizeGrow) {
    list.resize(5, 9);
    EXPECT_EQ(list.Size(), 5);
    EXPECT_EQ(list.back(), 9);
}

TEST_F(ListTest, ResizeShrink) {
    list.resize(2);
    EXPECT_EQ(list.Size(), 2);
    EXPECT_EQ(list.back(), 2);
}

TEST_F(ListTest, ComparisonEqual) {
    cont::List<int> other = list;
    EXPECT_TRUE(list == other);
    EXPECT_FALSE(list != other);
}

TEST_F(ListTest, ComparisonNotEqual) {
    cont::List<int> other;
    other.push_back(1);
    other.push_back(4);
    other.push_back(3);
    EXPECT_FALSE(list == other);
    EXPECT_TRUE(list != other);
}

TEST_F(ListTest, IteratorTraversal) {
    std::vector<int> expected = {1, 2, 3};
    auto it = list.begin();
    for (int val : expected) {
        ASSERT_NE(it, list.end());
        EXPECT_EQ(*it, val);
        ++it;
    }
    EXPECT_EQ(it, list.end());
}

TEST_F(ListTest, ReverseIteratorTraversal) {
    std::vector<int> expected = {3, 2, 1};
    auto rit = list.rend();
    --rit;
    for (int val : expected) {
        EXPECT_EQ(*rit, val);
        --rit;
    }
}

TEST_F(ListTest, ClearList) {
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.Size(), 0);
}

TEST(ListInitializerTest, InitList) {
    cont::List<int> l = {1, 2, 3, 4};
    EXPECT_EQ(l.Size(), 4);
    auto it = l.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
    EXPECT_EQ(*it++, 4);
    EXPECT_EQ(it, l.end());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
