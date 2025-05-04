#include <big_int.h>
#include <gtest/gtest.h>

TEST(BigIntEquality, BasicEquality) {
    EXPECT_TRUE(BigInt("0") == BigInt("0"));
    EXPECT_TRUE(BigInt("123456") == BigInt("123456"));
    EXPECT_FALSE(BigInt("123456") == BigInt("123457"));
    EXPECT_TRUE(BigInt("123") == BigInt("0123"));
    EXPECT_TRUE(BigInt("0000123") == BigInt("123"));
    EXPECT_FALSE(BigInt("0") == BigInt("1"));
    EXPECT_TRUE(BigInt("0000") == BigInt("0"));
}

TEST(BigIntEquality, DifferentLengths) {
    EXPECT_FALSE(BigInt("1") == BigInt("10"));
    EXPECT_FALSE(BigInt("123") == BigInt("1230"));
    EXPECT_FALSE(BigInt("999") == BigInt("999000"));
    EXPECT_TRUE(BigInt("000000123") == BigInt("123"));
}

TEST(BigIntLessThan, BasicCases) {
    EXPECT_TRUE(BigInt("0") < BigInt("1"));
    EXPECT_TRUE(BigInt("1") < BigInt("2"));
    EXPECT_TRUE(BigInt("999") < BigInt("1000"));
    EXPECT_TRUE(BigInt("123") < BigInt("124"));
    EXPECT_FALSE(BigInt("124") < BigInt("123"));
    EXPECT_FALSE(BigInt("1000") < BigInt("999"));
    EXPECT_FALSE(BigInt("999") < BigInt("999"));
}

TEST(BigIntLessThan, LeadingZeros) {
    EXPECT_FALSE(BigInt("0010") < BigInt("10"));
    EXPECT_TRUE(BigInt("0009") < BigInt("10"));
    EXPECT_FALSE(BigInt("0100") < BigInt("100"));
    EXPECT_TRUE(BigInt("0000999") < BigInt("1000"));
}

TEST(BigIntLessThan, DifferentLengths) {
    EXPECT_TRUE(BigInt("999") < BigInt("1000"));
    EXPECT_TRUE(BigInt("1") < BigInt("1000000000000000"));
    EXPECT_FALSE(BigInt("1000") < BigInt("999"));
    EXPECT_FALSE(BigInt("123456789") < BigInt("12345678"));
}

TEST(BigIntLessThan, EdgeDigits) {
    EXPECT_TRUE(BigInt("100000000") < BigInt("100000001"));
    EXPECT_FALSE(BigInt("100000001") < BigInt("100000000"));
    EXPECT_FALSE(BigInt("999999999") < BigInt("999999999"));
}

TEST(BigIntComparison, VeryLargeNumbers) {
    std::string a = "1" + std::string(1000, '0');
    std::string b = "1" + std::string(999, '0') + "1";
    EXPECT_FALSE(BigInt(b) < BigInt(a));
    EXPECT_TRUE(BigInt(a) < BigInt(b));
    EXPECT_FALSE(BigInt(a) == BigInt(b));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
