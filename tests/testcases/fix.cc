#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(Arithmetic, increment) {
	uint_t value(0);
	EXPECT_EQ(++value, 1);
	EXPECT_EQ(value++, 1);
	EXPECT_EQ(++value, 3);
}

TEST(Arithmetic, decrement) {
	uint_t value(0);
	EXPECT_EQ(--value, uint_t(0xffffffffffffffffULL));
	EXPECT_EQ(value--, uint_t(0xffffffffffffffffULL));
	EXPECT_EQ(--value, uint_t(0xfffffffffffffffdULL));
}
