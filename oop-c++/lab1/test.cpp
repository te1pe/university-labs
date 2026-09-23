#include <gtest/gtest.h>
#include "ICACHELRU.hpp"
#include "ICACHELFU.hpp"
#include "fibonachi.hpp"

TEST(CacheTest, LRU_BasicEviction) {
    LRU cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);

    EXPECT_EQ(cache.get(1), 1);

    cache.put(4, 4);

    EXPECT_EQ(cache.get(2), -1);
    EXPECT_EQ(cache.get(3), 3);
    EXPECT_EQ(cache.get(4), 4);
}

TEST(CacheTest, LRU_HardEvictionOrder) {
    LRU cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);

    EXPECT_EQ(cache.get(1), 1);
    cache.get(1);
    cache.get(1);
    cache.get(2);
    cache.get(3);

    cache.put(4, 4);

    EXPECT_EQ(cache.get(1), -1);
    EXPECT_NE(cache.get(2), -1);
    EXPECT_NE(cache.get(3), -1);
    EXPECT_NE(cache.get(4), -1);
}

TEST(CacheTest, LFU_BasicEviction) {
    LFU cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);

    EXPECT_EQ(cache.get(1), 1);
    EXPECT_EQ(cache.get(3), 3);

    cache.get(1);
    cache.get(3);

    cache.put(4, 4);

    EXPECT_EQ(cache.get(2), -1);
    EXPECT_EQ(cache.get(3), 3);
    EXPECT_EQ(cache.get(4), 4);
}

TEST(CacheTest, LFU_HardEvictionOrder) {
    LFU cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);

    EXPECT_EQ(cache.get(1), 1);
    cache.get(1);
    cache.get(2);
    cache.get(2);
    cache.get(3);

    cache.put(4, 4);

    EXPECT_EQ(cache.get(3), -1);
    EXPECT_NE(cache.get(1), -1);
    EXPECT_NE(cache.get(2), -1);
    EXPECT_NE(cache.get(4), -1);
}

TEST(CacheTest, LRU_GetFromEmptyAndMissing) {
    LRU cache(3);

    EXPECT_EQ(cache.get(42), -1);

    cache.put(1, 1);
    EXPECT_EQ(cache.get(2), -1);
}

TEST(CacheTest, LFU_GetFromEmptyAndMissing) {
    LFU cache(3);

    EXPECT_EQ(cache.get(42), -1);

    cache.put(1, 1);
    EXPECT_EQ(cache.get(2), -1);
}

TEST(CacheTest, LFU_CapacityZero) {
    LFU cache(0);

    cache.put(1, 1);
    EXPECT_EQ(cache.get(1), -1);
}

TEST(CacheTest, LRU_CapacityOne) {
    LRU cache(1);

    cache.put(1, 1);
    EXPECT_EQ(cache.get(1), 1);

    cache.put(2, 2);
    EXPECT_EQ(cache.get(1), -1);
    EXPECT_EQ(cache.get(2), 2);
}

TEST(CacheTest, LFU_CapacityOne) {
    LFU cache(1);

    cache.put(1, 1);
    EXPECT_EQ(cache.get(1), 1);

    cache.put(2, 2);
    EXPECT_EQ(cache.get(1), -1);
    EXPECT_EQ(cache.get(2), 2);
}

TEST(CacheTest, LRU_UpdateExistingKey) {
    LRU cache(2);
    cache.put(1, 1);
    cache.put(2, 2);

    cache.put(1, 10);

    EXPECT_EQ(cache.get(1), 10);
    EXPECT_EQ(cache.get(2), 2);
}

TEST(CacheTest, LFU_UpdateExistingKey) {
    LFU cache(2);
    cache.put(1, 1);
    cache.put(2, 2);

    cache.put(1, 10);

    EXPECT_EQ(cache.get(1), 10);
    EXPECT_EQ(cache.get(2), 2);
}

TEST(CacheTest, LRU_OperatorBracketAliasGet) {
    LRU cache(3);
    cache.put(1, 100);
    cache.put(2, 200);

    EXPECT_EQ(cache[1], 100);
    EXPECT_EQ(cache[2], 200);
    EXPECT_EQ(cache[3], -1);
}

TEST(CacheTest, LFU_OperatorBracketAliasGet) {
    LFU cache(3);
    cache.put(1, 100);
    cache.put(2, 200);

    EXPECT_EQ(cache[1], 100);
    EXPECT_EQ(cache[2], 200);
    EXPECT_EQ(cache[3], -1);
}

TEST(FibonachiTest, LRU_CorrectValues) {
    LRU cache(100);
    EXPECT_EQ(fibonachi(1, cache), 1);
    EXPECT_EQ(fibonachi(2, cache), 1);
    EXPECT_EQ(fibonachi(3, cache), 2);
    EXPECT_EQ(fibonachi(6, cache), 8);
    EXPECT_EQ(fibonachi(10, cache), 55);
}

TEST(FibonachiTest, LFU_CorrectValues) {
    LFU cache(100);
    EXPECT_EQ(fibonachi(1, cache), 1);
    EXPECT_EQ(fibonachi(2, cache), 1);
    EXPECT_EQ(fibonachi(3, cache), 2);
    EXPECT_EQ(fibonachi(6, cache), 8);
    EXPECT_EQ(fibonachi(10, cache), 55);
}
