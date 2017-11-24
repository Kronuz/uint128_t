#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(Arithmetic, subtract) {
	uinteger_t big  (0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL);
	uinteger_t small(1);
	uinteger_t zero(0);

	EXPECT_EQ(small - small, 0);
	EXPECT_EQ(small - big,   uinteger_t(0, 0, 0, 2));
	EXPECT_EQ(big   - small, uinteger_t(0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xfffffffffffffffeULL));
	EXPECT_EQ(big   - big,   0);
	EXPECT_EQ(zero  - small, uinteger_t(0xffffffffffffffffULL));
}

TEST(External, subtract) {
	bool     t   = true;
	bool     f   = false;
	uint8_t  u8  = 0xaaULL;
	uint16_t u16 = 0xaaaaULL;
	uint32_t u32 = 0xaaaaaaaaULL;
	uint64_t u64 = 0xaaaaaaaaaaaaaaaaULL;

	const uinteger_t val(0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL, 0xf0f0f0f0f0f0f0f0ULL);

	EXPECT_EQ(t   -  val, uinteger_t(0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f11ULL));
	EXPECT_EQ(f   -  val, uinteger_t(0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f10ULL));
	EXPECT_EQ(u8  -  val, uinteger_t(0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0fbaULL));
	EXPECT_EQ(u16 -  val, uinteger_t(0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0fb9baULL));
	EXPECT_EQ(u32 -  val, uinteger_t(0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0fb9b9b9baULL));
	EXPECT_EQ(u64 -  val, uinteger_t(0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0x0f0f0f0f0f0f0f0fULL, 0xb9b9b9b9b9b9b9baULL));

	EXPECT_EQ(t   -= val, true);
	EXPECT_EQ(f   -= val, true);
	EXPECT_EQ(u8  -= val, (uint8_t)  0xbaULL);
	EXPECT_EQ(u16 -= val, (uint16_t) 0xb9baULL);
	EXPECT_EQ(u32 -= val, (uint32_t) 0xb9b9b9baULL);
	EXPECT_EQ(u64 -= val, (uint64_t) 0xb9b9b9b9b9b9b9baULL);
}