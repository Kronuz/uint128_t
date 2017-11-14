#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(BitWise, or) {
	uint_t t  ((bool)     true);
	uint_t f  ((bool)     false);
	uint_t u8 ((uint8_t)  0xaaULL);
	uint_t u16((uint16_t) 0xaaaaULL);
	uint_t u32((uint32_t) 0xaaaaaaaaULL);
	uint_t u64((uint64_t) 0xaaaaaaaaaaaaaaaaULL);

	const uint_t val(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL);

	EXPECT_EQ(t   |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f1ULL));
	EXPECT_EQ(f   |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL));
	EXPECT_EQ(u8  |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0faULL));
	EXPECT_EQ(u16 |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0fafaULL));
	EXPECT_EQ(u32 |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0fafafafaULL));
	EXPECT_EQ(u64 |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xfafafafafafafafaULL));

	EXPECT_EQ(t   |= val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f1ULL));
	EXPECT_EQ(f   |= val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL));
	EXPECT_EQ(u8  |= val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0faULL));
	EXPECT_EQ(u16 |= val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0fafaULL));
	EXPECT_EQ(u32 |= val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0fafafafaULL));
	EXPECT_EQ(u64 |= val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xfafafafafafafafaULL));

	// zero
	EXPECT_EQ(uint_t(0) | val, val);
}

TEST(External, or) {
	bool     t   = true;
	bool     f   = false;
	uint8_t  u8  = 0xaaULL;
	uint16_t u16 = 0xaaaaULL;
	uint32_t u32 = 0xaaaaaaaaULL;
	uint64_t u64 = 0xaaaaaaaaaaaaaaaaULL;

	const uint_t val(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL);

	EXPECT_EQ(t   |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f1ULL));
	EXPECT_EQ(f   |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL));
	EXPECT_EQ(u8  |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0faULL));
	EXPECT_EQ(u16 |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0fafaULL));
	EXPECT_EQ(u32 |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0fafafafaULL));
	EXPECT_EQ(u64 |  val, uint_t(0xf0f0f0f0f0f0f0f0ULL, 0xfafafafafafafafaULL));

	EXPECT_EQ(t   |= val, true);
	EXPECT_EQ(f   |= val, true);
	EXPECT_EQ(u8  |= val, (uint8_t)  0xfaULL);
	EXPECT_EQ(u16 |= val, (uint16_t) 0xfafaULL);
	EXPECT_EQ(u32 |= val, (uint32_t) 0xfafafafaULL);
	EXPECT_EQ(u64 |= val, (uint64_t) 0xfafafafafafafafaULL);

	// zero
	EXPECT_EQ(uint_t(0) | val, val);
}
