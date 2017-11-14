#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(BitWise, invert) {
    EXPECT_EQ(~uint_t(0x0000000000000000ULL), uint_t(0x0000000000000001ULL));
    EXPECT_EQ(~uint_t(0xffffffffffffffffULL, 0xffffffffffffffffULL), uint_t(0x0000000000000000ULL));
    EXPECT_EQ(~uint_t(0xffffffffffffffffULL, 0x0000000000000000ULL, 0xffffffffffffffffULL), uint_t(0xffffffffffffffffULL, 0x0000000000000000ULL));
    EXPECT_EQ(~uint_t(0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL), uint_t(0x0000000000000000ULL));
    EXPECT_EQ(~uint_t(0xdeadbeefULL), 0x21524110ULL);
}
