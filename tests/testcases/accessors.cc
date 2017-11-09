#include <gtest/gtest.h>

#include "uint256_t.hh"

TEST(Accessor, bits) {
    uint256_t value = 1;
    for (size_t i = 0; i < 256; ++i) {
        EXPECT_EQ(value.bits(), i + 1);                     // before shift
        value <<= 1;
    }

    EXPECT_EQ(uint256_t(0).bits(), 0);
}

TEST(Accessor, data) {
    const uint256_t value(0xfedcba9876543210ULL, 0x0123456789abcdefULL);
    EXPECT_EQ(value[1], 0xfedcba9876543210ULL);
    EXPECT_EQ(value[0], 0x0123456789abcdefULL);
}
