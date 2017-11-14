#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(Comparison, not_equals) {
	EXPECT_EQ(!(uint_t(0xdeadbeefULL) != uint_t(0xdeadbeefULL)), true);
	EXPECT_EQ( (uint_t(0xdeadbeefULL) != uint_t(0xfee1baadULL)), true);
}

TEST(External, not_equals) {
	const bool     t   = true;
	const bool     f   = false;
	const uint8_t  u8  = 0xaaULL;
	const uint16_t u16 = 0xaaaaULL;
	const uint32_t u32 = 0xaaaaaaaaULL;
	const uint64_t u64 = 0xaaaaaaaaaaaaaaaaULL;

	EXPECT_EQ((t   != uint_t(f)),   true);
	EXPECT_EQ((f   != uint_t(t)),   true);
	EXPECT_EQ((u8  != uint_t(u64)), true);
	EXPECT_EQ((u16 != uint_t(u32)), true);
	EXPECT_EQ((u32 != uint_t(u16)), true);
	EXPECT_EQ((u64 != uint_t(u8)),  true);

	EXPECT_EQ((t   != uint_t(t)),   false);
	EXPECT_EQ((f   != uint_t(f)),   false);
	EXPECT_EQ((u8  != uint_t(u8)),  false);
	EXPECT_EQ((u16 != uint_t(u16)), false);
	EXPECT_EQ((u32 != uint_t(u32)), false);
	EXPECT_EQ((u64 != uint_t(u64)), false);
}
