#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(Arithmetic, increment) {
	uinteger_t value(0);
	EXPECT_EQ(++value, 1);
	EXPECT_EQ(value++, 1);
	EXPECT_EQ(++value, 3);
}

TEST(Arithmetic, decrement) {
	uinteger_t value(0);
	EXPECT_EQ(--value, uinteger_t(0xffffffffffffffffULL));
	EXPECT_EQ(value--, uinteger_t(0xffffffffffffffffULL));
	EXPECT_EQ(--value, uinteger_t(0xfffffffffffffffdULL));
}
