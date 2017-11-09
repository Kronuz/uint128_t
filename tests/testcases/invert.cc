#include <gtest/gtest.h>

#include "uint256_t.hh"

TEST(BitWise, invert) {
    EXPECT_EQ(~uint256_t(0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL), uint256_t(0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL));
    EXPECT_EQ(~uint256_t(0x0000000000000000ULL, 0x0000000000000000ULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL), uint256_t(0xffffffffffffffffULL, 0xffffffffffffffffULL, 0x0000000000000000ULL, 0x0000000000000000ULL));
    EXPECT_EQ(~uint256_t(0x0000000000000000ULL, 0xffffffffffffffffULL, 0x0000000000000000ULL, 0xffffffffffffffffULL), uint256_t(0xffffffffffffffffULL, 0x0000000000000000ULL, 0xffffffffffffffffULL, 0x0000000000000000ULL));
    EXPECT_EQ(~uint256_t(0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL), uint256_t(0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL));
}
