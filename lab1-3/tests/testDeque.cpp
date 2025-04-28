#include "deque.h"
#include <gtest/gtest.h>

class DqTest : public ::testing::Test {
protected:
    contDQ::Deque<int> deque = {1, 2, 3, 4, 5};

    // void SetUp() override {
    //     list.push_back(1);
    //     list.push_back(2);
    //     list.push_back(3);
    // }
};

TEST(Constructors, DefaultConstructor) {
    contDQ::Deque<int> dq;
    EXPECT_EQ(dq.size(), 0);
}

TEST(Constructors, ParamConstructor) {
    contDQ::Deque<int> dq = {1, 2, 3, 4};
    EXPECT_EQ(dq.size(), 4);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i++;
    }
}

TEST(Constructors, CopyConstructor) {
    contDQ::Deque<int> dq = {1, 3, 5, 7};
    contDQ::Deque<int> dq2 = dq;

    EXPECT_EQ(dq.size(), dq2.size());
    EXPECT_EQ(dq.size(), 4);
    int i = 1;
    for (int x: dq2) {
        EXPECT_EQ(x, i);
        i += 2;
    }
}

TEST(Constructors, MoveConstructor) {
    //contDQ::Deque<int> dq = contDQ::Deque<int>() = {1, 2, 3};
    contDQ::Deque<int> dq = {1, 3, 5, 7};
    contDQ::Deque<int> dq2 = std::move(dq);
    int i = 1;
    for (int x: dq2) {
        EXPECT_EQ(x, i);
        i += 2;
    }
}

TEST(Constructors, CopyEmptyDeque) {
    contDQ::Deque<int> dq;
    contDQ::Deque<int> dq2 = dq;
    EXPECT_EQ(dq2.size(), 0);
}

TEST(Constructors, MoveEmptyDeque) {
    contDQ::Deque<int> dq;
    contDQ::Deque<int> dq2 = std::move(dq);
    EXPECT_EQ(dq2.size(), 0);
}

TEST(operatorEq, SelfAssignment) {
    contDQ::Deque<int> dq = {1, 2, 3};
    dq = dq;
    EXPECT_EQ(dq.size(), 3);
}

TEST_F(DqTest, atEmpty) {
    contDQ::Deque<int> dq;
    EXPECT_THROW(dq.at(0), std::out_of_range);
}

TEST_F(DqTest, operatorAtEmpty) {
    contDQ::Deque<int> dq;
    EXPECT_THROW(dq[0], std::out_of_range);
}

TEST_F(DqTest, insertEmpty) {
    contDQ::Deque<int> dq;
    dq.insert(dq.cbegin(), 1);
    EXPECT_EQ(dq.size(), 1);
    EXPECT_EQ(dq.front(), 1);
}

TEST_F(DqTest, eraseSingleElement) {
    contDQ::Deque<int> dq = {1};
    dq.erase(dq.cbegin());
    EXPECT_EQ(dq.size(), 0);
}

TEST_F(DqTest, eraseEmpty) {
    contDQ::Deque<int> dq;
    EXPECT_THROW(dq.erase(dq.cbegin()), std::out_of_range);
}

TEST_F(DqTest, resizeToZero) {
    contDQ::Deque<int> dq = {1, 2, 3};
    EXPECT_THROW(dq.resize(0, 0), std::out_of_range);
}

TEST_F(DqTest, resizeEmpty) {
    contDQ::Deque<int> dq;
    dq.resize(3, 42);
    EXPECT_EQ(dq.size(), 3);
    EXPECT_EQ(dq.front(), 42);
}

TEST(compare, differentLengths) {
    contDQ::Deque<int> dq1 = {1, 2, 3};
    contDQ::Deque<int> dq2 = {1, 2};
    EXPECT_FALSE(dq1 == dq2);
    EXPECT_TRUE(dq1 != dq2);
}

TEST(compare, spaceshipAllCases) {
    contDQ::Deque<int> dq1 = {1, 2, 3};
    contDQ::Deque<int> dq2 = {1, 2, 4};
    contDQ::Deque<int> dq3 = {1, 2, 3};
    EXPECT_TRUE((dq1 <=> dq2) == std::strong_ordering::less);
    EXPECT_TRUE((dq2 <=> dq1) == std::strong_ordering::greater);
    EXPECT_TRUE((dq1 <=> dq3) == std::strong_ordering::equal);
}

TEST_F(DqTest, swapWithEmpty) {
    contDQ::Deque<int> dq1 = {1, 2, 3};
    contDQ::Deque<int> dq2;
    dq1.swap(dq2);
    EXPECT_TRUE(dq1.empty());
    EXPECT_EQ(dq2.size(), 3);
}

TEST_F(DqTest, swapSelf) {
    deque.swap(deque);
    EXPECT_EQ(deque.size(), 5);
}

TEST_F(DqTest, resizeToOne) {
    deque.resize(1, 0);
    EXPECT_EQ(deque.size(), 1);
    EXPECT_EQ(deque.front(), 1);
}

TEST(operatorEq, CopyOperator) {
    contDQ::Deque<int> dq = {1, 3, 5, 7};
    contDQ::Deque<int> dq2;
    dq2 = dq;
    EXPECT_EQ(dq.size(), dq2.size());
    EXPECT_EQ(dq.size(), 4);
    int i = 1;
    for (int x: dq2) {
        EXPECT_EQ(x, i);
        i += 2;
    }
}

TEST(operatorEq, MoveOperator) {
    contDQ::Deque<int> dq = {1, 3, 5, 7};
    contDQ::Deque<int> dq2;
    dq2 = std::move(dq);
    EXPECT_EQ(dq.size(), dq2.size());
    EXPECT_EQ(dq.size(), 4);
    int i = 1;
    for (int x: dq2) {
        EXPECT_EQ(x, i);
        i += 2;
    }
}

TEST_F(DqTest, at) {
    int a = deque.at(2);
    EXPECT_EQ(a, 3);
}

TEST_F(DqTest, atExc) {
    EXPECT_THROW(deque.at(5), std::out_of_range);
}

TEST_F(DqTest, operatorAt) {
    EXPECT_EQ(deque[2], 3);
}

TEST_F(DqTest, operatorAtOut) {
    EXPECT_THROW(deque[5], std::out_of_range);
}

TEST_F(DqTest, front) {
    EXPECT_EQ(deque.front(), 1);
    deque.front() = 10;
    EXPECT_EQ(deque.front(), 10);
}

TEST_F(DqTest, frontExc) {
    contDQ::Deque<int> dq;
    EXPECT_THROW(dq.front(), std::out_of_range);
}

TEST_F(DqTest, back) {
    EXPECT_EQ(deque.back(), 5);
    deque.back() = 10;
    EXPECT_EQ(deque.back(), 10);
}

TEST_F(DqTest, backExc) {
    contDQ::Deque<int> dq;
    EXPECT_THROW(dq.back(), std::out_of_range);
}

TEST_F(DqTest, Iterator) {
    auto it = deque.begin();
    int i = 1;
    for (; it != deque.end(); it++) {
        EXPECT_EQ(*it, i);
        i += 1;
    }

    i = 1;
    for (; it != deque.end(); ++it) {
        EXPECT_EQ(*it, i);
        i += 1;
    }

    auto it2 = deque.begin();
    it2++;
    ++it2;
    it2--;
    --it2;
    EXPECT_EQ(*it2, 1);
}

TEST_F(DqTest, constIterator) {
    auto it = deque.cbegin();
    int i = 1;
    for (; it != deque.cend(); it++) {
        EXPECT_EQ(*it, i);
        i += 1;
    }

    i = 1;
    for (; it != deque.cend(); ++it) {
        EXPECT_EQ(*it, i);
        i += 1;
    }

    auto it2 = deque.begin();
    it2++;
    ++it2;
    it2--;
    --it2;
    EXPECT_EQ(*it2, 1);
}

TEST_F(DqTest, revIterator) {
    auto it = deque.rbegin();
    int i = 5;
    for (; it != deque.rend(); it++) {
        EXPECT_EQ(*it, i);
        i -= 1;
    }

    i = 1;
    for (; it != deque.rend(); ++it) {
        EXPECT_EQ(*it, i);
        i -= 1;
    }

    auto it2 = deque.rbegin();
    it2++;
    ++it2;
    it2--;
    --it2;
    EXPECT_EQ(*it2, 5);
}

TEST_F(DqTest, ConstRevIterator) {
    auto it = deque.crbegin();
    int i = 5;
    for (; it != deque.crend(); it++) {
        EXPECT_EQ(*it, i);
        i -= 1;
    }

    i = 1;
    for (; it != deque.crend(); ++it) {
        EXPECT_EQ(*it, i);
        i -= 1;
    }

    auto it2 = deque.crbegin();
    it2++;
    ++it2;
    it2--;
    --it2;
    EXPECT_EQ(*it2, 5);
}

TEST_F(DqTest, empty) {
    EXPECT_EQ(deque.empty(), false);
    contDQ::Deque<int> dq;
    EXPECT_EQ(dq.empty(), true);
}

TEST_F(DqTest, max_size) {
    EXPECT_EQ(deque.max_size(), 5);
    contDQ::Deque<int> dq;
    EXPECT_EQ(dq.max_size(), 0);
}

TEST_F(DqTest, clear) {
    contDQ::Deque<int> dq = {1, 2, 3};
    dq.clear();
    EXPECT_EQ(dq.size(), 0);
    EXPECT_THROW(dq.front(), std::out_of_range);
    EXPECT_THROW(dq.back(), std::out_of_range);
}

TEST_F(DqTest, insertBegin) {
    contDQ::Deque<int> dq = {1, 2, 3};
    dq.insert(dq.cbegin(), 0);
    EXPECT_EQ(dq.size(), 4);
    int i = 0;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, insertMiddle) {
    contDQ::Deque<int> dq = {1, 2, 4};
    dq.insert(dq.cbegin().next(2), 3);
    EXPECT_EQ(dq.size(), 4);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, insertEnd) {
    contDQ::Deque<int> dq = {1, 2, 3};
    dq.insert(dq.cend(), 4);
    EXPECT_EQ(dq.size(), 4);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, eraseBegin) {
    contDQ::Deque<int> dq = {1, 2, 3, 4};
    dq.erase(dq.cbegin());
    EXPECT_EQ(dq.size(), 3);
    int i = 2;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, eraseMiddle) {
    contDQ::Deque<int> dq = {1, 2, 10, 3};
    dq.erase(dq.cbegin().next(2));
    EXPECT_EQ(dq.size(), 3);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, eraseEnd) {
    contDQ::Deque<int> dq = {1, 2, 3, 10};
    dq.erase(dq.cbegin().next(3));
    EXPECT_EQ(dq.size(), 3);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, pushBack) {
    contDQ::Deque<int> dq = {1, 2, 3, 4};
    dq.push_back(5);
    EXPECT_EQ(dq.size(), 5);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, pushFront) {
    contDQ::Deque<int> dq = {2, 3, 4};
    dq.push_front(1);
    EXPECT_EQ(dq.size(), 4);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, popBack) {
    contDQ::Deque<int> dq = {1, 2, 3, 4};
    dq.pop_back();
    EXPECT_EQ(dq.size(), 3);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, popBackExc) {
    contDQ::Deque<int> dq;
    EXPECT_THROW(dq.pop_back(), std::out_of_range);
}

TEST_F(DqTest, popFrontExc) {
    contDQ::Deque<int> dq;
    EXPECT_THROW(dq.pop_front(), std::out_of_range);
}

TEST_F(DqTest, resizeLess) {
    contDQ::Deque<int> dq = {1, 2, 3, 4};
    dq.resize(2, 0);
    EXPECT_EQ(dq.size(), 2);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, resizeMore) {
    contDQ::Deque<int> dq = {1, 2, 3, 4};
    dq.resize(10, 0);
    EXPECT_EQ(dq.size(), 10);
    int i = 1;
    for (int x: dq) {
        if (i <= 4) {
            EXPECT_EQ(x, i);
            i += 1;
        } else {
            EXPECT_EQ(x, 0);
        }
    }
}

TEST_F(DqTest, resizeEq) {
    contDQ::Deque<int> dq = {1, 2, 3, 4};
    dq.resize(4, 0);
    EXPECT_EQ(dq.size(), 4);
    int i = 1;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 1;
    }
}

TEST_F(DqTest, swap) {
    contDQ::Deque<int> dq = {1, 2, 3, 4};
    contDQ::Deque<int> dq1 = {10, 20};

    dq1.swap(dq);
    EXPECT_EQ(dq1.size(), 4);
    int i = 1;
    for (int x: dq1) {
        EXPECT_EQ(x, i);
        i += 1;
    }

    i = 10;
    for (int x: dq) {
        EXPECT_EQ(x, i);
        i += 10;
    }
}

TEST(compare, equality) {
    contDQ::Deque<int> dq1 = {1, 2, 3, 4};
    contDQ::Deque<int> dq2 = {1, 2, 3, 4};
    EXPECT_TRUE(dq1 == dq2);
}

TEST(compare, unequality) {
    contDQ::Deque<int> dq1 = {1, 2, 3, 4};
    contDQ::Deque<int> dq2 = {1, 2, 3};
    contDQ::Deque<int> dq3 = {1, 2, 3, 5};
    EXPECT_TRUE(dq1 != dq2);
    EXPECT_TRUE(dq1 != dq3);
}

TEST(compare, spaceship) {
    contDQ::Deque<int> dq1 = {1, 2, 3, 4};
    contDQ::Deque<int> dq2 = {1, 2, 3};
    contDQ::Deque<int> dq3 = {1, 2, 3, 5};
    EXPECT_FALSE(dq1 <= dq2);
    EXPECT_TRUE(dq1 <= dq3);
    EXPECT_TRUE(dq1 < dq3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
