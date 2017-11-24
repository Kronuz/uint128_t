#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(BitWise, and) {
	uinteger_t t  ((bool)     true);
	uinteger_t f  ((bool)     false);
	uinteger_t u8 ((uint8_t)  0xaaULL);
	uinteger_t u16((uint16_t) 0xaaaaULL);
	uinteger_t u32((uint32_t) 0xaaaaaaaaULL);
	uinteger_t u64((uint64_t) 0xaaaaaaaaaaaaaaaaULL);

	const uinteger_t val(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL);

	EXPECT_EQ(t   &  val, uinteger_t(0));
	EXPECT_EQ(f   &  val, uinteger_t(0));
	EXPECT_EQ(u8  &  val, uinteger_t(0xa0ULL));
	EXPECT_EQ(u16 &  val, uinteger_t(0xa0a0ULL));
	EXPECT_EQ(u32 &  val, uinteger_t(0xa0a0a0a0ULL));
	EXPECT_EQ(u64 &  val, uinteger_t(0xa0a0a0a0a0a0a0a0ULL));

	EXPECT_EQ(t   &= val, uinteger_t(0x0ULL));
	EXPECT_EQ(f   &= val, uinteger_t(0x0ULL));
	EXPECT_EQ(u8  &= val, uinteger_t(0xa0ULL));
	EXPECT_EQ(u16 &= val, uinteger_t(0xa0a0ULL));
	EXPECT_EQ(u32 &= val, uinteger_t(0xa0a0a0a0ULL));
	EXPECT_EQ(u64 &= val, uinteger_t(0xa0a0a0a0a0a0a0a0ULL));
}

TEST(External, and) {
	bool     t   = true;
	bool     f   = false;
	uint8_t  u8  = 0xaaULL;
	uint16_t u16 = 0xaaaaULL;
	uint32_t u32 = 0xaaaaaaaaULL;
	uint64_t u64 = 0xaaaaaaaaaaaaaaaaULL;

	const uinteger_t val(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL);

	EXPECT_EQ(t   &  val, uinteger_t(0x0ULL));
	EXPECT_EQ(f   &  val, uinteger_t(0x0ULL));
	EXPECT_EQ(u8  &  val, uinteger_t(0xa0ULL));
	EXPECT_EQ(u16 &  val, uinteger_t(0xa0a0ULL));
	EXPECT_EQ(u32 &  val, uinteger_t(0xa0a0a0a0ULL));
	EXPECT_EQ(u64 &  val, uinteger_t(0xa0a0a0a0a0a0a0a0ULL));

	EXPECT_EQ(t   &= val, false);
	EXPECT_EQ(f   &= val, false);
	EXPECT_EQ(u8  &= val, (uint8_t)  0xa0ULL);
	EXPECT_EQ(u16 &= val, (uint16_t) 0xa0a0ULL);
	EXPECT_EQ(u32 &= val, (uint32_t) 0xa0a0a0a0ULL);
	EXPECT_EQ(u64 &= val, (uint64_t) 0xa0a0a0a0a0a0a0a0ULL);

	// zero
	EXPECT_EQ(uinteger_t(0) & val, 0);
}
