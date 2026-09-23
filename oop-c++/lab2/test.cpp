#include <gtest/gtest.h>
#include "bitarr.h"

TEST(BitArrayTest, DefaultConstructor) {
    BitArray array;

    EXPECT_EQ(array.size(), 0);
    EXPECT_TRUE(array.empty());
    EXPECT_TRUE(array.none());
    EXPECT_EQ(array.count(), 0);
    EXPECT_EQ(array.to_string(), "");
}

TEST(BitArrayTest, ValueConstructor) {
    BitArray array(10, 5);

    EXPECT_EQ(array.size(), 10);
    EXPECT_FALSE(array.empty());
    EXPECT_EQ(array.to_string(), "1010000000");
    EXPECT_THROW((BitArray(-1)), std::invalid_argument);
}

TEST(BitArrayTest, ValueConstructorCutsExtraBits) {
    BitArray array(3, 0xFF);

    EXPECT_EQ(array.size(), 3);
    EXPECT_EQ(array.count(), 3);
    EXPECT_EQ(array.to_string(), "111");
}

TEST(BitArrayTest, CopyAndAssign) {
    BitArray source(5, 0b10101);
    BitArray copy(source);
    BitArray assigned;
    assigned = source;

    EXPECT_TRUE(copy == source);
    EXPECT_TRUE(assigned == source);

    copy.set(1);
    EXPECT_TRUE(copy != source);
}

TEST(BitArrayTest, SetAndAccess) {
    BitArray array(5);

    array.set(2);
    EXPECT_TRUE(array[2]);

    array[3] = true;
    EXPECT_TRUE(array[3]);

    array.set(3, false);
    EXPECT_FALSE(array[3]);

    array.set();
    EXPECT_EQ(array.count(), 5);
}

TEST(BitArrayTest, ResetWholeAndSingle) {
    BitArray array(5);
    array.set();

    array.reset(2);
    EXPECT_FALSE(array[2]);
    EXPECT_EQ(array.count(), 4);

    array.reset();
    EXPECT_TRUE(array.none());
}

TEST(BitArrayTest, ReferenceFlip) {
    BitArray array(4);

    array[1].flip();
    EXPECT_TRUE(array[1]);

    array[1].flip();
    EXPECT_FALSE(array[1]);

    array[0] = array[1];
    EXPECT_FALSE(array[0]);
}

TEST(BitArrayTest, AnyNoneCount) {
    BitArray array(5);

    EXPECT_FALSE(array.any());
    EXPECT_TRUE(array.none());

    array.set(0);
    EXPECT_TRUE(array.any());
    EXPECT_FALSE(array.none());
    EXPECT_EQ(array.count(), 1);

    array.set(4);
    EXPECT_EQ(array.count(), 2);
}

TEST(BitArrayTest, PushBack) {
    BitArray array;

    array.push_back(true);
    array.push_back(false);
    array.push_back(true);

    EXPECT_EQ(array.size(), 3);
    EXPECT_EQ(array.to_string(), "101");
}

TEST(BitArrayTest, PushBackAcrossBlockBorder) {
    BitArray array;
    for (int i = 0; i < 130; ++i) {
        array.push_back(i % 3 == 0);
    }

    EXPECT_EQ(array.size(), 130);
    EXPECT_EQ(array.count(), 44);
    EXPECT_TRUE(array[129]);
}

TEST(BitArrayTest, ResizeGrowAndShrink) {
    BitArray array(4, 0b1111);

    array.resize(6, true);
    EXPECT_EQ(array.to_string(), "111111");

    array.resize(8, false);
    EXPECT_EQ(array.to_string(), "11111100");

    array.resize(3);
    EXPECT_EQ(array.to_string(), "111");

    EXPECT_THROW(array.resize(-2), std::invalid_argument);
}

TEST(BitArrayTest, ResizeDropsOldBitsCompletely) {
    BitArray array(70);
    array.set();

    array.resize(5);
    array.resize(70);

    EXPECT_EQ(array.count(), 5);
}

TEST(BitArrayTest, BitwiseAssignOperators) {
    BitArray left(5, 0b10101);
    BitArray right(5, 0b01110);

    BitArray conj(left);
    conj &= right;
    EXPECT_EQ(conj.to_string(), "00100");

    BitArray disj(left);
    disj |= right;
    EXPECT_EQ(disj.to_string(), "11111");

    BitArray excl(left);
    excl ^= right;
    EXPECT_EQ(excl.to_string(), "11011");
}

TEST(BitArrayTest, BitwiseOperatorsRejectDifferentSizes) {
    BitArray left(5);
    BitArray right(6);

    EXPECT_THROW(left &= right, std::invalid_argument);
    EXPECT_THROW(left |= right, std::invalid_argument);
    EXPECT_THROW(left ^= right, std::invalid_argument);
    EXPECT_THROW(left & right, std::invalid_argument);
    EXPECT_THROW(left | right, std::invalid_argument);
    EXPECT_THROW(left ^ right, std::invalid_argument);
}

TEST(BitArrayTest, FreeBitwiseOperatorsKeepArguments) {
    BitArray left(4, 0b1100);
    BitArray right(4, 0b1010);

    EXPECT_EQ((left & right).to_string(), "0001");
    EXPECT_EQ((left | right).to_string(), "0111");
    EXPECT_EQ((left ^ right).to_string(), "0110");
    EXPECT_EQ(left.to_string(), "0011");
    EXPECT_EQ(right.to_string(), "0101");
}

TEST(BitArrayTest, Shifts) {
    BitArray array(8, 0b00001111);

    EXPECT_EQ((array << 2).to_string(), "00111100");
    EXPECT_EQ((array >> 2).to_string(), "11000000");
    EXPECT_EQ((array << 0).to_string(), array.to_string());
    EXPECT_EQ((array << 8).to_string(), "00000000");
    EXPECT_EQ((array >> 100).to_string(), "00000000");
}

TEST(BitArrayTest, ShiftWithNegativeArgumentChangesDirection) {
    BitArray array(8, 0b00001111);

    EXPECT_EQ((array << -2).to_string(), (array >> 2).to_string());
    EXPECT_EQ((array >> -2).to_string(), (array << 2).to_string());
}

TEST(BitArrayTest, ShiftAcrossBlockBorder) {
    BitArray array(200);
    array.set(0);
    array.set(1);

    array <<= 70;
    EXPECT_EQ(array.count(), 2);
    EXPECT_TRUE(array[70]);
    EXPECT_TRUE(array[71]);

    array >>= 70;
    EXPECT_TRUE(array[0]);
    EXPECT_TRUE(array[1]);
}

TEST(BitArrayTest, BitwiseNot) {
    BitArray array(5, 0b10101);

    EXPECT_EQ((~array).to_string(), "01010");
    EXPECT_EQ((~array).count(), 2);
}

TEST(BitArrayTest, NotKeepsTailClean) {
    BitArray array(65);

    EXPECT_EQ((~array).count(), 65);
}

TEST(BitArrayTest, ComparisonOperators) {
    BitArray first(5, 0b10101);
    BitArray second(5, 0b10101);
    BitArray third(5, 0b01010);

    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);
    EXPECT_TRUE(first != third);
    EXPECT_FALSE(first == BitArray(4, 0b1010));
}

TEST(BitArrayTest, SwapAndClear) {
    BitArray first(5, 0b10101);
    BitArray second(5, 0b01010);

    first.swap(second);
    EXPECT_EQ(first.to_string(), "01010");
    EXPECT_EQ(second.to_string(), "10101");

    first.clear();
    EXPECT_EQ(first.size(), 0);
    EXPECT_TRUE(first.empty());
}

TEST(BitArrayTest, ToString) {
    BitArray array(8, 0b11001100);

    EXPECT_EQ(array.to_string(), "00110011");
}

TEST(BitArrayTest, IndexOutOfBounds) {
    BitArray array(5);
    const BitArray& readonly = array;

    EXPECT_THROW(array.set(-1), std::out_of_range);
    EXPECT_THROW(array.set(5), std::out_of_range);
    EXPECT_THROW(array[-1], std::out_of_range);
    EXPECT_THROW(array[5], std::out_of_range);
    EXPECT_THROW(readonly[5], std::out_of_range);
}
