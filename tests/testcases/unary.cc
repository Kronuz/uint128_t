#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(Arithmetic, unary_plus) {
	const uint_t value(0x12345ULL);
	EXPECT_EQ(+value, value);
}

TEST(Arithmetic, unary_minus) {
	const uint_t val(1);
	const uint_t neg = -val;

	EXPECT_EQ(-val, neg);
	EXPECT_EQ(-neg, val);
	EXPECT_EQ(neg, uint_t(0xffffffffffffffffULL));
}