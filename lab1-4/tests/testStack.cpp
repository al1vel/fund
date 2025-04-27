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

TEST(Constructors, Default) {
    st::Stack<int> stack;
    EXPECT_EQ(stack.size(), 0);
    EXPECT_THROW(stack.pop(), std::out_of_range);
}

TEST(Constructors, Init) {
    st::Stack<int> stack {1, 2, 3, 4};
    EXPECT_EQ(stack.size(), 4);
    int i = 4;
    for (; i > 0; i--) {
        EXPECT_EQ(stack.top(), i);
        stack.pop();
    }
}

TEST(Constructors, Copy) {
    st::Stack<int> stack = {1, 2, 3};
    st::Stack<int> copy = stack;
    EXPECT_EQ(copy.size(), 3);
    int i = 3;
    for (; i > 0; i--) {
        EXPECT_EQ(copy.top(), i);
        copy.pop();
    }
}

TEST(Constructors, Move) {
    st::Stack<int> stack = {1, 2, 3};
    st::Stack<int> copy = std::move(stack);
    EXPECT_EQ(copy.size(), 3);
    int i = 3;
    for (; i > 0; i--) {
        EXPECT_EQ(copy.top(), i);
        copy.pop();
    }
}

TEST(AssignmentOperators, CopyAssignment) {
    st::Stack<int> s1 = {5, 6, 7};
    st::Stack<int> s2;
    s2 = s1;

    EXPECT_EQ(s2.size(), 3);
    int i = 7;
    for (; i > 4; i--) {
        EXPECT_EQ(s2.top(), i);
        s2.pop();
    }
}

TEST(AssignmentOperators, MoveAssignment) {
    st::Stack<int> s1 = {5, 6, 7};
    st::Stack<int> s2;
    s2 = std::move(s1);

    EXPECT_EQ(s2.size(), 3);
    int i = 7;
    for (; i > 4; i--) {
        EXPECT_EQ(s2.top(), i);
        s2.pop();
    }
}

TEST(StackTest, PushIncreasesSize) {
    st::Stack<int> s;
    s.push(42);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s.top(), 42);
    s.push(10);
    EXPECT_EQ(s.size(), 2);
    EXPECT_EQ(s.top(), 10);
}

TEST(StackTest, PopDecreasesSize) {
    st::Stack<int> s;
    s.push(1);
    s.push(2);
    s.pop();
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s.top(), 1);
}

TEST(StackTest, Empty) {
    st::Stack<int> s;
    EXPECT_TRUE(s.empty());
    s.push(42);
    EXPECT_FALSE(s.empty());
}

TEST(StackTest, popExc) {
    st::Stack<int> s;
    EXPECT_THROW(s.pop(), std::out_of_range);
}

TEST(StackTest, SwapStacks) {
    st::Stack<int> s1 = {1, 2};
    st::Stack<int> s2 = {3, 4, 5};

    s1.swap(s2);

    EXPECT_EQ(s1.size(), 3);
    EXPECT_EQ(s1.top(), 5);

    EXPECT_EQ(s2.size(), 2);
    EXPECT_EQ(s2.top(), 2);
}

TEST(CompareTest, ComparisonOperators) {
    st::Stack<int> s1 = {1, 2, 3};
    st::Stack<int> s2 = {1, 2, 3};
    st::Stack<int> s3 = {1, 2};

    EXPECT_TRUE(s1 == s2);
    EXPECT_FALSE(s1 != s2);
    EXPECT_TRUE(s1 != s3);
}

TEST(CompareTest, ThreeWayComparison) {
    st::Stack<int> s1 = {1, 2, 3};
    st::Stack<int> s2 = {1, 2, 3};
    st::Stack<int> s3 = {1, 2, 4};

    EXPECT_EQ(s1 <=> s2, std::strong_ordering::equal);
    EXPECT_EQ(s1 <=> s3, std::strong_ordering::less);
    EXPECT_EQ(s3 <=> s1, std::strong_ordering::greater);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}