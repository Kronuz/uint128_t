#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(Logical, and) {
	const uinteger_t A(0xffffffff);
	const uinteger_t B(0x00000000);

	EXPECT_EQ(A && A, true);
	EXPECT_EQ(A && B, false);
}

TEST(Logical, or) {
	const uinteger_t A(0xffffffff);
	const uinteger_t B(0x00000000);

	EXPECT_EQ(A || A, true);
	EXPECT_EQ(A || B, true);
}

TEST(Logical, not) {
	EXPECT_EQ(!uinteger_t(0xffffffff), 0);
}
