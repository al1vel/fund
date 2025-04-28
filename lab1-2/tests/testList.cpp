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

TEST(Constructors, Default) {
    cont::List<int> l;
    ASSERT_EQ(l.size(), 0);
}

TEST(Constructors, InitList) {
    cont::List<int> l = {1, 2, 3, 4};
    EXPECT_EQ(l.size(), 4);
    auto it = l.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
    EXPECT_EQ(*it++, 4);
    EXPECT_EQ(it, l.end());
}

TEST(Constructors, copy_constructor) {
    cont::List<int> l = {1, 2, 3};
    cont::List<int> l1 = l;
    EXPECT_EQ(l1.front(), 1);
    EXPECT_EQ(l1.back(), 3);
    EXPECT_EQ(l1.size(), 3);
}

TEST(Constructors, move_constructor) {
    cont::List<int> l = {1, 2, 3};
    cont::List<int> l1 = std::move(l);
    EXPECT_EQ(l1.front(), 1);
    EXPECT_EQ(l1.back(), 3);
}

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
    EXPECT_EQ(list.size(), 4);

    cont::List<int> l;
    l.push_front(1);
    EXPECT_EQ(l.front(), 1);
    EXPECT_EQ(l.back(), 1);
    EXPECT_EQ(l.size(), 1);
}

TEST_F(ListTest, PopFront) {
    list.pop_front();
    EXPECT_EQ(list.front(), 2);
    EXPECT_EQ(list.size(), 2);

    cont::List<int> l;
    EXPECT_THROW(l.pop_front(), std::out_of_range);
}

TEST_F(ListTest, PushBack) {
    list.push_back(4);
    EXPECT_EQ(list.back(), 4);
    EXPECT_EQ(list.size(), 4);
}

TEST_F(ListTest, PopBack) {
    list.pop_back();
    EXPECT_EQ(list.back(), 2);
    EXPECT_EQ(list.size(), 2);

    cont::List<int> l;
    EXPECT_THROW(l.pop_back(), std::out_of_range);
}

TEST_F(ListTest, Insert) {
    auto it = list.cbegin().next(); // указывает на 2
    list.insert(it, 42);
    auto itCheck = list.begin();
    ++itCheck;
    EXPECT_EQ(*itCheck, 42);
    EXPECT_EQ(list.size(), 4);
}

TEST_F(ListTest, Erase) {
    auto it = list.cbegin().next(); // указывает на 2
    list.erase(it);
    auto itCheck = ++list.begin();
    EXPECT_EQ(*itCheck, 3);
    EXPECT_EQ(list.size(), 2);

    cont::List<int> l = {1, 2, 3, 4, 5};
    l.erase(l.cbegin());
    EXPECT_EQ(l.size(), 4);
    EXPECT_EQ(l.front(), 2);

    l.erase(l.cbegin().next(3));
    EXPECT_EQ(l.back(), 4);
    EXPECT_EQ(l.size(), 3);
}

TEST_F(ListTest, ResizeGrow) {
    list.resize(5, 9);
    EXPECT_EQ(list.size(), 5);
    EXPECT_EQ(list.back(), 9);
}

TEST_F(ListTest, ResizeShrink) {
    list.resize(2, 0);
    EXPECT_EQ(list.size(), 2);
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

TEST_F(ListTest, ClearList) {
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST(Compare, spaceship) {
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

TEST_F(ListTest, insert2) {
    cont::List<int> l = {1, 2, 3};
    l.insert(l.cbegin(), 10);
    EXPECT_EQ(l.front(), 10);
    EXPECT_EQ(l.back(), 3);
    EXPECT_EQ(l.size(), 4);
}

TEST_F(ListTest, swap) {
    cont::List<int> l = {1, 2, 3};
    cont::List<int> l1 = {4, 5, 6, 7};
    l.swap(l1);
    EXPECT_EQ(l.front(), 4);
    EXPECT_EQ(l.back(), 7);
    EXPECT_EQ(l.size(), 4);

    EXPECT_EQ(l1.front(), 1);
    EXPECT_EQ(l1.back(), 3);
    EXPECT_EQ(l1.size(), 3);
}

TEST_F(ListTest, compEq) {
    cont::List<int> l = {1, 2, 3, 4};
    EXPECT_FALSE(l == list);
}

TEST_F(ListTest, resizeBad) {
    cont::List<int> l;
    EXPECT_THROW(l.resize(0, 0), std::out_of_range);
}

TEST_F(ListTest, resize0) {
    cont::List<int> l = {1, 2};
    l.resize(2, 0);
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

TEST(Iterators, Iterator) {
    cont::List<int> l = {1, 2, 3, 4};
    auto it = l.begin();
    int i = 1;
    for (; it != l.end(); ++it) {
        EXPECT_EQ(*it, i);
        ++i;
    }

    auto it2 = l.begin().next(3);
    i = 4;
    for (; it2 != l.begin(); --it2) {
        EXPECT_EQ(*it2, i);
        --i;
    }

    auto it3 = l.begin();
    it3++;
    it3++;
    it3--;
    it3--;
    EXPECT_EQ(*it3, 1);
}

TEST(Iterators, ConstIterator) {
    cont::List<int> l = {1, 2, 3, 4};
    auto it = l.cbegin();
    int i = 1;
    for (; it != l.cend(); ++it) {
        EXPECT_EQ(*it, i);
        ++i;
    }

    auto it2 = l.cbegin().next(3);
    i = 4;
    for (; it2 != l.cbegin(); --it2) {
        EXPECT_EQ(*it2, i);
        --i;
    }

    auto it3 = l.cbegin();
    it3++;
    it3++;
    it3--;
    it3--;
    EXPECT_EQ(*it3, 1);
}

TEST(Iterators, ReverseIterator) {
    cont::List<int> l = {1, 2, 3, 4};
    auto it = l.rbegin();
    int i = 4;
    for (; it != l.rend(); ++it) {
        EXPECT_EQ(*it, i);
        --i;
    }

    auto it3 = l.rbegin();
    it3++;
    it3++;
    it3--;
    --it3;
    EXPECT_EQ(*it3, 4);

    auto c = l.rbegin();
    auto d = l.rbegin();
    EXPECT_TRUE(c == d);
}

TEST(Iterators, ConstRevIterator) {
    cont::List<int> l = {1, 2, 3, 4};
    auto it = l.crbegin();
    int i = 4;
    for (; it != l.crend(); ++it) {
        EXPECT_EQ(*it, i);
        --i;
    }

    auto it3 = l.crbegin();
    it3++;
    it3++;
    --it3;
    it3--;
    EXPECT_EQ(*it3, 4);
}

TEST(AssignmentOperators, CopyAssignment) {
    cont::List<int> l;
    cont::List<int> l1 = {1, 2, 3};
    l1 = l;
    EXPECT_EQ(l1.size(), 0);
}

TEST_F(ListTest, maxSize) {
    EXPECT_EQ(list.max_size(), 3);
}

TEST_F(ListTest, insertEnd) {
    list.insert(list.cend(), 4);
    int i = 1;
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        EXPECT_EQ(*it, i);
        i++;
    }
}

TEST_F(ListTest, pop1) {
    cont::List<int> l = {1};
    l.pop_back();
    EXPECT_EQ(l.size(), 0);
    EXPECT_THROW(l.front(), std::out_of_range);
}

TEST_F(ListTest, pop2) {
    cont::List<int> l = {1};
    l.pop_front();
    EXPECT_EQ(l.size(), 0);
    EXPECT_THROW(l.front(), std::out_of_range);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
