#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(Logical, and) {
    const uint_t A(0xffffffff);
    const uint_t B(0x00000000);

    EXPECT_EQ(A && A, true);
    EXPECT_EQ(A && B, false);
}

TEST(Logical, or) {
    const uint_t A(0xffffffff);
    const uint_t B(0x00000000);

    EXPECT_EQ(A || A, true);
    EXPECT_EQ(A || B, true);
}

TEST(Logical, not) {
    EXPECT_EQ(!uint_t(0xffffffff), 0);
}
