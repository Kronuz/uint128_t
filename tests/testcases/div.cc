#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(Arithmetic, divide) {
	const uinteger_t big_val  (0xfedbca9876543210ULL);
	const uinteger_t small_val(0xffffULL);
	const uinteger_t res_val  (0xfedcc9753fc9ULL);

	EXPECT_EQ(small_val / small_val, 1);
	EXPECT_EQ(small_val / big_val,   0);

	EXPECT_EQ(big_val   / big_val,   1);

	EXPECT_THROW(uinteger_t(1) / uinteger_t(0), std::domain_error);
}

TEST(External, divide) {
	bool     t   = true;
	bool     f   = false;
	uint8_t  u8  = 0xaaULL;
	uint16_t u16 = 0xaaaaULL;
	uint32_t u32 = 0xaaaaaaaaULL;
	uint64_t u64 = 0xaaaaaaaaaaaaaaaaULL;

	const uinteger_t val(0x7bULL);

	EXPECT_EQ(t   /  val, false);
	EXPECT_EQ(f   /  val, false);
	EXPECT_EQ(u8  /  val, uinteger_t(0x1ULL));
	EXPECT_EQ(u16 /  val, uinteger_t(0x163ULL));
	EXPECT_EQ(u32 /  val, uinteger_t(0x163356bULL));
	EXPECT_EQ(u64 /  val, uinteger_t(0x163356b88ac0de0ULL));

	EXPECT_EQ(t   /= val, false);
	EXPECT_EQ(f   /= val, false);
	EXPECT_EQ(u8  /= val, (uint8_t)  0x1ULL);
	EXPECT_EQ(u16 /= val, (uint16_t) 0x163ULL);
	EXPECT_EQ(u32 /= val, (uint32_t) 0x163356bULL);
	EXPECT_EQ(u64 /= val, (uint64_t) 0x163356b88ac0de0ULL);
}
