#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(Arithmetic, add) {
	uinteger_t low (0, 1);
	uinteger_t high(1, 0);

	EXPECT_EQ(low  + low,  2);
	EXPECT_EQ(low  + high, uinteger_t(1, 1));
	EXPECT_EQ(high + high, uinteger_t(2, 0));

	EXPECT_EQ(low  += low,  2);
	EXPECT_EQ(low  += high, uinteger_t(1, 2));
	EXPECT_EQ(high += low,  uinteger_t(2, 2));
}

TEST(External, add) {
	bool     t   = true;
	bool     f   = false;
	uint8_t  u8  = 0xaaULL;
	uint16_t u16 = 0xaaaaULL;
	uint32_t u32 = 0xaaaaaaaaULL;
	uint64_t u64 = 0xaaaaaaaaaaaaaaaaULL;

	const uinteger_t val(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL);

	EXPECT_EQ(t   +  val, uinteger_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f1ULL));
	EXPECT_EQ(f   +  val, uinteger_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL));
	EXPECT_EQ(u8  +  val, uinteger_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f19aULL));
	EXPECT_EQ(u16 +  val, uinteger_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f19b9aULL));
	EXPECT_EQ(u32 +  val, uinteger_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f19b9b9b9aULL));
	EXPECT_EQ(u64 +  val, uinteger_t(0xf0f0f0f0f0f0f0f1ULL, 0x9b9b9b9b9b9b9b9aULL));

	EXPECT_EQ(t   += val, true);
	EXPECT_EQ(f   += val, true);
	EXPECT_EQ(u8  += val, (uint8_t)  0x9aULL);
	EXPECT_EQ(u16 += val, (uint16_t) 0x9b9aULL);
	EXPECT_EQ(u32 += val, (uint32_t) 0x9b9b9b9aULL);
	EXPECT_EQ(u64 += val, (uint64_t) 0x9b9b9b9b9b9b9b9aULL);
}
