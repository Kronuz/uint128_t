#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(BitShift, left1) {
    // operator<<
    uint_t val(0x1);
    uint64_t exp_val = 1;

    for (uint8_t i = 0; i < 64; ++i) {
        EXPECT_EQ(val << i, exp_val << i);
    }

    uint_t zero(0);
    for (uint8_t i = 0; i < 64; ++i) {
        EXPECT_EQ(zero << i, 0);
    }

    for (size_t j = 0; j < 10; ++j) {
        for (size_t i = 0; i < 64; ++i) {
            auto bits = (j * 64 + i);
            auto final = val << bits;
            EXPECT_EQ(final[j], 1ULL << i);
            EXPECT_EQ(final.bits(), bits + 1);
        }
    }
}

TEST(BitShift, left2) {
    uint_t val(0x1);
    uint64_t exp_val = 1;

    // operator<<=
    for (uint8_t i = 0; i < 63; ++i) { // 1 is already a bit
        EXPECT_EQ(val <<= 1, exp_val <<= 1);
    }

    uint_t zero(0);
    for (uint8_t i = 0; i < 63; ++i) {
        EXPECT_EQ(zero <<= 1, 0);
    }

    val = 1;
    for (size_t j = 0; j < 10; ++j) {
        for (size_t i = 0; i < 64; ++i) {
            auto bits = (j * 64 + i);
            EXPECT_EQ(val[j], 1ULL << i);
            EXPECT_EQ(val.bits(), bits + 1);
            val <<= 1;
        }
    }
}

TEST(External, shift_left_zero) {
    bool     t   = true;
    bool     f   = false;
    uint8_t  u8  = 0xffULL;
    uint16_t u16 = 0xffffULL;
    uint32_t u32 = 0xffffffffULL;
    uint64_t u64 = 0xffffffffffffffffULL;

    const uint_t zero(0);
    EXPECT_EQ(t   << zero, t);
    EXPECT_EQ(f   << zero, f);
    EXPECT_EQ(u8  << zero, u8);
    EXPECT_EQ(u16 << zero, u16);
    EXPECT_EQ(u32 << zero, u32);
    EXPECT_EQ(u64 << zero, u64);

    EXPECT_EQ(t   <<= zero, t);
    EXPECT_EQ(f   <<= zero, f);
    EXPECT_EQ(u8  <<= zero, u8);
    EXPECT_EQ(u16 <<= zero, u16);
    EXPECT_EQ(u32 <<= zero, u32);
    EXPECT_EQ(u64 <<= zero, u64);
}

TEST(External, shift_left_one) {
    bool     t   = true;
    bool     f   = false;
    uint8_t  u8  = 0xffULL;
    uint16_t u16 = 0xffffULL;
    uint32_t u32 = 0xffffffffULL;
    uint64_t u64 = 0xffffffffffffffffULL;

    const uint_t one(1);
    EXPECT_EQ(t   << one, uint_t(t)   << 1);
    EXPECT_EQ(f   << one, uint_t(f)   << 1);
    EXPECT_EQ(u8  << one, uint_t(u8)  << 1);
    EXPECT_EQ(u16 << one, uint_t(u16) << 1);
    EXPECT_EQ(u32 << one, uint_t(u32) << 1);
    EXPECT_EQ(u64 << one, uint_t(u64) << 1);

    EXPECT_EQ(t   <<= one, true);
    EXPECT_EQ(f   <<= one, false);
    EXPECT_EQ(u8  <<= one, (uint8_t)  0xfeULL);
    EXPECT_EQ(u16 <<= one, (uint16_t) 0xfffeULL);
    EXPECT_EQ(u32 <<= one, (uint32_t) 0xfffffffeULL);
    EXPECT_EQ(u64 <<= one, (uint64_t) 0xfffffffffffffffeULL);
}

TEST(External, shift_left) {
    uint8_t  u8  = 0xffULL;
    uint16_t u16 = 0xffffULL;
    uint32_t u32 = 0xffffffffULL;
    uint64_t u64 = 0xffffffffffffffffULL;

    EXPECT_EQ(u8  << uint_t(7),  uint_t(0x7f80ULL));
    EXPECT_EQ(u16 << uint_t(15), uint_t(0x7fff8000ULL));
    EXPECT_EQ(u32 << uint_t(31), uint_t(0x7fffffff80000000ULL));
    EXPECT_EQ(u64 << uint_t(63), uint_t(0x7fffffffffffffff, 0x8000000000000000ULL));
    EXPECT_EQ(u8  <<= uint_t(7),  (uint8_t)  0x80);
    EXPECT_EQ(u16 <<= uint_t(15), (uint16_t) 0x8000);
    EXPECT_EQ(u32 <<= uint_t(31), (uint32_t) 0x80000000);
    EXPECT_EQ(u64 <<= uint_t(63), (uint64_t) 0x8000000000000000);
}
