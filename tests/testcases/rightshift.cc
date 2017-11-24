#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(BitShift, right1) {
	// operator>>
	uinteger_t val(0xffffffffffffffffULL);
	uint64_t exp_val = 0xffffffffffffffffULL;

	for (uint8_t i = 0; i < 64; ++i) {
		EXPECT_EQ(val >> i, exp_val >> i);
	}

	uinteger_t zero(0);
	for (uint8_t i = 0; i < 64; ++i) {
		EXPECT_EQ(zero >> i, 0);
	}

	val = uinteger_t(1) << 640;
	for (size_t j = 0; j < 10; ++j) {
		for (size_t i = 0; i < 64; ++i) {
			auto bits = (j * 64 + i);
			auto final = val >> bits;
			EXPECT_EQ(final.bits(), 640 - bits + 1);
		}
	}
}

TEST(BitShift, right2) {
	// operator>>=
	uinteger_t val(0xffffffffffffffffULL);
	uint64_t exp_val = 0xffffffffffffffffULL;

	for (uint8_t i = 0; i < 64; ++i) {
		EXPECT_EQ(val >>= 1, exp_val >>= 1);
	}

	uinteger_t zero(0);
	for (uint8_t i = 0; i < 64; ++i) {
		EXPECT_EQ(zero >>= 1, 0);
	}

	val = uinteger_t(1) << 640;
	for (size_t j = 0; j < 10; ++j) {
		for (size_t i = 0; i < 64; ++i) {
			auto bits = (j * 64 + i);
			EXPECT_EQ(val.bits(), 640 - bits + 1);
			val >>= 1;
		}
	}
}

TEST(External, shift_right) {
	bool     t   = true;
	bool     f   = false;
	uint8_t  u8  = 0xffULL;
	uint16_t u16 = 0xffffULL;
	uint32_t u32 = 0xffffffffULL;
	uint64_t u64 = 0xffffffffffffffffULL;

	const uinteger_t zero(0);
	const uinteger_t one(1);

	EXPECT_EQ(t   >> zero, one);
	EXPECT_EQ(f   >> zero, zero);
	EXPECT_EQ(u8  >> zero, u8);
	EXPECT_EQ(u16 >> zero, u16);
	EXPECT_EQ(u32 >> zero, u32);
	EXPECT_EQ(u64 >> zero, u64);

	EXPECT_EQ(t   >>= zero, t);
	EXPECT_EQ(f   >>= zero, f);
	EXPECT_EQ(u8  >>= zero, u8);
	EXPECT_EQ(u16 >>= zero, u16);
	EXPECT_EQ(u32 >>= zero, u32);
	EXPECT_EQ(u64 >>= zero, u64);

	EXPECT_EQ(t   >> one, uinteger_t(t)   >> 1);
	EXPECT_EQ(f   >> one, uinteger_t(f)   >> 1);
	EXPECT_EQ(u8  >> one, uinteger_t(u8)  >> 1);
	EXPECT_EQ(u16 >> one, uinteger_t(u16) >> 1);
	EXPECT_EQ(u32 >> one, uinteger_t(u32) >> 1);
	EXPECT_EQ(u64 >> one, uinteger_t(u64) >> 1);

	EXPECT_EQ(t   >>= one, false);
	EXPECT_EQ(f   >>= one, false);
	EXPECT_EQ(u8  >>= one, (uint8_t)  0x7fULL);
	EXPECT_EQ(u16 >>= one, (uint16_t) 0x7fffULL);
	EXPECT_EQ(u32 >>= one, (uint32_t) 0x7fffffffULL);
	EXPECT_EQ(u64 >>= one, (uint64_t) 0x7fffffffffffffffULL);

	EXPECT_EQ(u8  >> uinteger_t(7),  zero);
	EXPECT_EQ(u16 >> uinteger_t(15), zero);
	EXPECT_EQ(u32 >> uinteger_t(31), zero);
	EXPECT_EQ(u64 >> uinteger_t(63), zero);

	EXPECT_EQ(u8  >>= uinteger_t(7),  (uint8_t)  0);
	EXPECT_EQ(u16 >>= uinteger_t(15), (uint16_t) 0);
	EXPECT_EQ(u32 >>= uinteger_t(31), (uint32_t) 0);
	EXPECT_EQ(u64 >>= uinteger_t(63), (uint64_t) 0);
}