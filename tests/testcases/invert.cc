#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(BitWise, invert) {
	EXPECT_EQ(~uinteger_t(0x0000000000000000ULL), uinteger_t(0x0000000000000001ULL));
	EXPECT_EQ(~uinteger_t(0xffffffffffffffffULL, 0xffffffffffffffffULL), uinteger_t(0x0000000000000000ULL));
	EXPECT_EQ(~uinteger_t(0xffffffffffffffffULL, 0x0000000000000000ULL, 0xffffffffffffffffULL), uinteger_t(0xffffffffffffffffULL, 0x0000000000000000ULL));
	EXPECT_EQ(~uinteger_t(0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL), uinteger_t(0x0000000000000000ULL));
	EXPECT_EQ(~uinteger_t(0xdeadbeefULL), 0x21524110ULL);
}
