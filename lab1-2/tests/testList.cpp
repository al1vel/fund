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

    cont::List<int> l;
    EXPECT_THROW(l.front(), std::out_of_range);
    EXPECT_THROW(l.back(), std::out_of_range);
}

TEST_F(ListTest, PushFront) {
    list.push_front(0);
    EXPECT_EQ(list.front(), 0);
    EXPECT_EQ(list.Size(), 4);

    cont::List<int> l;
    l.push_front(1);
    EXPECT_EQ(l.front(), 1);
    EXPECT_EQ(l.back(), 1);
    EXPECT_EQ(l.Size(), 1);
}

TEST_F(ListTest, PopFront) {
    list.pop_front();
    EXPECT_EQ(list.front(), 2);
    EXPECT_EQ(list.Size(), 2);

    cont::List<int> l;
    EXPECT_THROW(l.pop_front(), std::out_of_range);
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

    cont::List<int> l;
    EXPECT_THROW(l.pop_back(), std::out_of_range);
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

    cont::List<int> l = {1, 2, 3, 4, 5};
    l.erase(l.cbegin());
    EXPECT_EQ(l.Size(), 4);
    EXPECT_EQ(l.front(), 2);

    l.erase(l.cbegin().next(3));
    EXPECT_EQ(l.back(), 4);
    EXPECT_EQ(l.Size(), 3);
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
    auto rit = list.rbegin();
        for (int val : expected) {
        EXPECT_EQ(*rit, val);
        ++rit;
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

TEST_F(ListTest, spaceship) {
    cont::List<int> l1 = {1, 2, 3};
    cont::List<int> l2 = {1, 2, 4};
    cont::List<int> l3 = {1, 2, 3};

    EXPECT_EQ((l1 < l2), true);
    EXPECT_EQ((l1 > l2), false);
    EXPECT_EQ((l1 <= l3), true);
    EXPECT_EQ((l1 >= l3), true);
}

TEST_F(ListTest, assignment_operator) {
    cont::List<int> l;
    l = list;
    EXPECT_EQ(l.front(), 1);
    EXPECT_EQ(l.back(), 3);
}

TEST_F(ListTest, move_operator) {
    cont::List<int> l = {1, 2, 3};
    cont::List<int> l1;
    l1 = std::move(l);
    EXPECT_EQ(l1.front(), 1);
    EXPECT_EQ(l1.back(), 3);
}

TEST_F(ListTest, move_constructor) {
    cont::List<int> l = {1, 2, 3};
    cont::List<int> l1 = std::move(l);
    EXPECT_EQ(l1.front(), 1);
    EXPECT_EQ(l1.back(), 3);
}

TEST_F(ListTest, iter) {
    cont::List<int> l = {1, 2, 3};
    auto it = l.begin().next(2);
    --it;
    it--;
    EXPECT_EQ(*it, 1);
}

TEST_F(ListTest, insert2) {
    cont::List<int> l = {1, 2, 3};
    l.insert(l.cbegin(), 10);
    EXPECT_EQ(l.front(), 10);
    EXPECT_EQ(l.back(), 3);
    EXPECT_EQ(l.Size(), 4);
}

TEST_F(ListTest, swap) {
    cont::List<int> l = {1, 2, 3};
    cont::List<int> l1 = {4, 5, 6, 7};
    l.swap(l1);
    EXPECT_EQ(l.front(), 4);
    EXPECT_EQ(l.back(), 7);
    EXPECT_EQ(l.Size(), 4);

    EXPECT_EQ(l1.front(), 1);
    EXPECT_EQ(l1.back(), 3);
    EXPECT_EQ(l1.Size(), 3);
}

TEST_F(ListTest, compEq) {
    cont::List<int> l = {1, 2, 3, 4};
    EXPECT_FALSE(l == list);
}

TEST_F(ListTest, resizeBad) {
    cont::List<int> l;
    EXPECT_THROW(l.resize(0), std::out_of_range);
}

TEST_F(ListTest, resize0) {
    cont::List<int> l = {1, 2};
    l.resize(2);
    EXPECT_EQ(l.front(), 1);
    EXPECT_EQ(l.back(), 2);
}

TEST_F(ListTest, resizeFrom0) {
    cont::List<int> l;
    l.resize(2, 10);
    EXPECT_EQ(l.front(), 10);
    EXPECT_EQ(l.back(), 10);
}

TEST_F(ListTest, assSame) {
    cont::List<int> l = {1, 2, 3};
    l = l;
    EXPECT_EQ(l.front(), 1);
    EXPECT_EQ(l.back(), 3);
}

TEST_F(ListTest, nextBad) {
    cont::List<int> l = {1, 2, 3};
    EXPECT_THROW(l.cbegin().next(4), std::out_of_range);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
