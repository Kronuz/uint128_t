#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(Constructor, standard) {
    uint_t value(0x0123456789abcdefULL);
    const uint_t original = value;

    EXPECT_EQ(uint_t(0), 0);
    EXPECT_EQ(value, original);
    EXPECT_EQ(uint_t(value), original);
    EXPECT_EQ(uint_t(std::move(value)), original);
}

TEST(Constructor, one) {
    EXPECT_EQ(uint_t(true)[1],  false);
    EXPECT_EQ(uint_t(true)[0],   true);
    EXPECT_EQ(uint_t(false)[1], false);
    EXPECT_EQ(uint_t(false)[0], false);

    EXPECT_EQ(uint_t((uint8_t)  0x01ULL)[1],               0ULL);
    EXPECT_EQ(uint_t((uint16_t) 0x0123ULL)[1],             0ULL);
    EXPECT_EQ(uint_t((uint32_t) 0x01234567ULL)[1],         0ULL);
    EXPECT_EQ(uint_t((uint64_t) 0x0123456789abcdefULL)[1], 0ULL);

    EXPECT_EQ(uint_t((uint8_t)  0x01ULL)[0],               (uint8_t)  0x01ULL);
    EXPECT_EQ(uint_t((uint16_t) 0x0123ULL)[0],             (uint16_t) 0x0123ULL);
    EXPECT_EQ(uint_t((uint32_t) 0x01234567ULL)[0],         (uint32_t) 0x01234567ULL);
    EXPECT_EQ(uint_t((uint64_t) 0x0123456789abcdefULL)[0], (uint64_t) 0x0123456789abcdefULL);
}

TEST(Constructor, two) {
    for (uint8_t hi = 0; hi < 2; ++hi) {
        for (uint8_t lo = 0; lo < 2; ++lo) {
            const uint_t val(hi, lo);
            EXPECT_EQ(val[1], hi);
            EXPECT_EQ(val[0], lo);
        }
    }

    EXPECT_EQ(uint_t((uint8_t)  0x01ULL,               (uint8_t)  0x01ULL)[1],               (uint8_t)  0x01ULL);
    EXPECT_EQ(uint_t((uint16_t) 0x0123ULL,             (uint16_t) 0x0123ULL)[1],             (uint16_t) 0x0123ULL);
    EXPECT_EQ(uint_t((uint32_t) 0x01234567ULL,         (uint32_t) 0x01234567ULL)[1],         (uint32_t) 0x01234567ULL);
    EXPECT_EQ(uint_t((uint64_t) 0x0123456789abcdefULL, (uint64_t) 0x0123456789abcdefULL)[1], (uint64_t) 0x0123456789abcdefULL);

    EXPECT_EQ(uint_t((uint8_t)  0x01ULL,               (uint8_t)  0x01ULL)[0],               (uint8_t)  0x01ULL);
    EXPECT_EQ(uint_t((uint16_t) 0x0123ULL,             (uint16_t) 0x0123ULL)[0],             (uint16_t) 0x0123ULL);
    EXPECT_EQ(uint_t((uint32_t) 0x01234567ULL,         (uint32_t) 0x01234567ULL)[0],         (uint32_t) 0x01234567ULL);
    EXPECT_EQ(uint_t((uint64_t) 0x0123456789abcdefULL, (uint64_t) 0x0123456789abcdefULL)[0], (uint64_t) 0x0123456789abcdefULL);
}

TEST(Constructor, string) {
    EXPECT_EQ(uint_t("1010", 2), 10ULL);
    EXPECT_EQ(uint_t("033", 8), 27ULL);
    EXPECT_EQ(uint_t("1492", 10), 1492ULL);
    EXPECT_EQ(uint_t("1492"), 1492ULL);
    EXPECT_EQ(uint_t("000f", 16), 15ULL);

    EXPECT_EQ(uint_t("\330\105\140\310\23\117\21\346\241\342\64\66\73\322\155\256", 256).str(), "287473478535710989927820548052301802926");
    EXPECT_EQ(uint_t(std::string("\330\105\140\310\23\117\21\346\241\342\64\66\73\322\155\256", 16), 256).str(), "287473478535710989927820548052301802926");
    const char* uuid1 = "\330\105\140\310\23\117\21\346\241\342\64\66\73\322\155\256";
    EXPECT_EQ(uint_t(uuid1, 256).str(), "287473478535710989927820548052301802926");
    const char uuid2[] = "\330\105\140\310\23\117\21\346\241\342\64\66\73\322\155\256";
    EXPECT_EQ(uint_t(uuid2, 256).str(), "287473478535710989927820548052301802926");
}
