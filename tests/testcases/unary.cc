#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(Arithmetic, unary_plus) {
	const uinteger_t value(0x12345ULL);
	EXPECT_EQ(+value, value);
}

TEST(Arithmetic, unary_minus) {
	const uinteger_t val(1);
	const uinteger_t neg = -val;

	EXPECT_EQ(-val, neg);
	EXPECT_EQ(-neg, val);
	EXPECT_EQ(neg, uinteger_t(0xffffffffffffffffULL));
}