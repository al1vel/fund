#include "unique_ptr.h"
#include <gtest/gtest.h>
#include <memory>
#include <utility>

class TestObject {
private:
    int value;

public:
    TestObject() : value(0) { ++count; }
    explicit TestObject(int v) : value(v) { ++count; }
    ~TestObject() { --count; }

    int getValue() const { return value; }
    void setValue(int v) { value = v; }

    static int count;
};

int TestObject::count = 0;

struct TestDeleter {
    void operator()(TestObject *p) {
        delete p;
        deleted = true;
    }

    static bool deleted;
};

bool TestDeleter::deleted = false;

struct ArrayTestDeleter {
    void operator()(int *p) {
        delete[] p;
        deleted = true;
    }

    static bool deleted;
};

bool ArrayTestDeleter::deleted = false;

class UniquePtrTest : public ::testing::Test {
protected:
    void SetUp() override {
        TestObject::count = 0;
        TestDeleter::deleted = false;
        ArrayTestDeleter::deleted = false;
    }

    void TearDown() override {
        ASSERT_EQ(TestObject::count, 0);
    }
};

TEST_F(UniquePtrTest, DefaultConstructor) {
    smart_ptr::Unique_ptr<TestObject> ptr;
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(ptr));
}

TEST_F(UniquePtrTest, PointerConstructor) { {
        TestObject *raw = new TestObject(42);
        smart_ptr::Unique_ptr<TestObject> ptr(raw);
        EXPECT_EQ(ptr.get(), raw);
        EXPECT_EQ(ptr->getValue(), 42);
        EXPECT_EQ((*ptr).getValue(), 42);
        EXPECT_TRUE(static_cast<bool>(ptr));
    }
    EXPECT_EQ(TestObject::count, 0);
}

TEST_F(UniquePtrTest, MoveConstructor) {
    TestObject *raw = new TestObject(42);
    smart_ptr::Unique_ptr<TestObject> ptr1(raw);
    smart_ptr::Unique_ptr<TestObject> ptr2(std::move(ptr1));

    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(ptr2.get(), raw);
    EXPECT_FALSE(static_cast<bool>(ptr1));
    EXPECT_TRUE(static_cast<bool>(ptr2));
}

TEST_F(UniquePtrTest, CustomDeleter) { {
        TestObject *raw = new TestObject(42);
        smart_ptr::Unique_ptr<TestObject, TestDeleter> ptr(raw);
        EXPECT_FALSE(TestDeleter::deleted);
    }
    EXPECT_TRUE(TestDeleter::deleted);
}

TEST_F(UniquePtrTest, MoveAssignment) {
    TestObject *raw1 = new TestObject(42);
    TestObject *raw2 = new TestObject(24);

    smart_ptr::Unique_ptr<TestObject> ptr1(raw1);
    smart_ptr::Unique_ptr<TestObject> ptr2(raw2);

    ptr2 = std::move(ptr1);

    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(ptr2.get(), raw1);
    EXPECT_EQ(TestObject::count, 1);
}

TEST_F(UniquePtrTest, Reset) {
    TestObject *raw1 = new TestObject(42);
    TestObject *raw2 = new TestObject(24);

    smart_ptr::Unique_ptr<TestObject> ptr(raw1);
    ptr.reset(raw2);

    EXPECT_EQ(ptr.get(), raw2);
    EXPECT_EQ(TestObject::count, 1);

    ptr.reset();
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(TestObject::count, 0);
}

TEST_F(UniquePtrTest, ResetWithCustomDeleter) {
    TestObject *raw = new TestObject(42);
    smart_ptr::Unique_ptr<TestObject, TestDeleter> ptr(raw);

    ptr.reset();
    EXPECT_TRUE(TestDeleter::deleted);
    EXPECT_EQ(ptr.get(), nullptr);
}

TEST_F(UniquePtrTest, Release) {
    TestObject *raw = new TestObject(42);
    smart_ptr::Unique_ptr<TestObject> ptr(raw);

    TestObject *released = ptr.release();

    EXPECT_EQ(released, raw);
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(TestObject::count, 1);

    delete released;
}

TEST_F(UniquePtrTest, Swap) {
    TestObject *raw1 = new TestObject(42);
    TestObject *raw2 = new TestObject(24);

    smart_ptr::Unique_ptr<TestObject> ptr1(raw1);
    smart_ptr::Unique_ptr<TestObject> ptr2(raw2);

    ptr1.swap(ptr2);

    EXPECT_EQ(ptr1.get(), raw2);
    EXPECT_EQ(ptr2.get(), raw1);
    EXPECT_EQ(TestObject::count, 2);
}

TEST_F(UniquePtrTest, ArrayDefaultConstructor) {
    smart_ptr::Unique_ptr<int[]> ptr;
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(ptr));
}

TEST_F(UniquePtrTest, ArrayPointerConstructor) {
    int *raw = new int[3]{1, 2, 3};
    smart_ptr::Unique_ptr<int[]> ptr(raw);

    EXPECT_EQ(ptr.get(), raw);
    EXPECT_EQ(ptr[0], 1);
    EXPECT_EQ(ptr[1], 2);
    EXPECT_EQ(ptr[2], 3);
    EXPECT_TRUE(static_cast<bool>(ptr));
}

TEST_F(UniquePtrTest, ArrayMoveConstructor) {
    int *raw = new int[3]{1, 2, 3};
    smart_ptr::Unique_ptr<int[]> ptr1(raw);
    smart_ptr::Unique_ptr<int[]> ptr2(std::move(ptr1));

    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(ptr2.get(), raw);
    EXPECT_FALSE(static_cast<bool>(ptr1));
    EXPECT_TRUE(static_cast<bool>(ptr2));
}

TEST_F(UniquePtrTest, ArrayCustomDeleter) {
    int *raw = new int[3]{1, 2, 3}; {
        smart_ptr::Unique_ptr<int[], ArrayTestDeleter> ptr(raw);
        EXPECT_FALSE(ArrayTestDeleter::deleted);
    }
    EXPECT_TRUE(ArrayTestDeleter::deleted);
}

TEST_F(UniquePtrTest, ArrayMoveAssignment) {
    int *raw1 = new int[3]{1, 2, 3};
    int *raw2 = new int[2]{4, 5};

    smart_ptr::Unique_ptr<int[]> ptr1(raw1);
    smart_ptr::Unique_ptr<int[]> ptr2(raw2);

    ptr2 = std::move(ptr1);

    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(ptr2.get(), raw1);
}

TEST_F(UniquePtrTest, ArrayReset) {
    int *raw1 = new int[3]{1, 2, 3};
    int *raw2 = new int[2]{4, 5};

    smart_ptr::Unique_ptr<int[]> ptr(raw1);
    ptr.reset(raw2);

    EXPECT_EQ(ptr.get(), raw2);

    ptr.reset();
    EXPECT_EQ(ptr.get(), nullptr);
}

TEST_F(UniquePtrTest, ArrayRelease) {
    int *raw = new int[3]{1, 2, 3};
    smart_ptr::Unique_ptr<int[]> ptr(raw);

    int *released = ptr.release();

    EXPECT_EQ(released, raw);
    EXPECT_EQ(ptr.get(), nullptr);

    delete[] released;
}

TEST_F(UniquePtrTest, ArraySwap) {
    int *raw1 = new int[3]{1, 2, 3};
    int *raw2 = new int[2]{4, 5};

    smart_ptr::Unique_ptr<int[]> ptr1(raw1);
    smart_ptr::Unique_ptr<int[]> ptr2(raw2);

    ptr1.swap(ptr2);

    EXPECT_EQ(ptr1.get(), raw2);
    EXPECT_EQ(ptr2.get(), raw1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
