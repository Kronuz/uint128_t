#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(Arithmetic, multiply) {
	uint_t val(0xfedbca9876543210ULL);

	EXPECT_EQ(val * val, uint_t(0xfdb8e2bacbfe7cefULL, 0x010e6cd7a44a4100ULL));

	const uint_t zero = 0;
	EXPECT_EQ(val  * zero, zero);
	EXPECT_EQ(zero * val,  zero);

	const uint_t one = 1;
	EXPECT_EQ(val * one, val);
	EXPECT_EQ(one * val, val);
}

TEST(External, multiply) {
	bool     t   = true;
	bool     f   = false;
	uint8_t  u8  = 0xaaULL;
	uint16_t u16 = 0xaaaaULL;
	uint32_t u32 = 0xaaaaaaaaULL;
	uint64_t u64 = 0xaaaaaaaaaaaaaaaaULL;

	const uint_t val(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL);

	EXPECT_EQ(t   *  val, val);
	EXPECT_EQ(f   *  val, 0);
	EXPECT_EQ(u8  *  val, uint_t(0x9fULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffff60ULL));
	EXPECT_EQ(u16 *  val, uint_t(0xa09fULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffff5f60ULL));
	EXPECT_EQ(u32 *  val, uint_t(0xa0a0a09f, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffff5f5f5f60ULL));
	EXPECT_EQ(u64 *  val, uint_t(0xa0a0a0a0a0a0a09fULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0x5f5f5f5f5f5f5f60ULL));

	EXPECT_EQ(t   *= val, true);
	EXPECT_EQ(f   *= val, false);
	EXPECT_EQ(u8  *= val, (uint8_t)                0x60ULL);
	EXPECT_EQ(u16 *= val, (uint16_t)             0x5f60ULL);
	EXPECT_EQ(u32 *= val, (uint32_t)         0x5f5f5f60ULL);
	EXPECT_EQ(u64 *= val, (uint64_t) 0x5f5f5f5f5f5f5f60ULL);
}

TEST(External, inplace) {
	uint_t val = 0;
	for (const auto& i : {1, 29, 15, 57, 12, 45}) {
		val *= 58;
		val += i;
	}
	EXPECT_EQ(val, 0x3ade68b1);
}
