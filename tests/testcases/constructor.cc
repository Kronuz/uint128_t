#include <random>
#include <algorithm>
#include <functional>

#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(Constructor, standard) {
	uinteger_t value(0x0123456789abcdefULL);
	const uinteger_t original = value;

	EXPECT_EQ(uinteger_t(0), 0);
	EXPECT_EQ(value, original);
	EXPECT_EQ(uinteger_t(value), original);
	EXPECT_EQ(uinteger_t(std::move(value)), original);
}

TEST(Constructor, one) {
	EXPECT_EQ(uinteger_t(true).value(1),  false);
	EXPECT_EQ(uinteger_t(true).value(0),   true);
	EXPECT_EQ(uinteger_t(false).value(1), false);
	EXPECT_EQ(uinteger_t(false).value(0), false);

	EXPECT_EQ(uinteger_t((uint8_t)  0x01ULL).value(1),               0ULL);
	EXPECT_EQ(uinteger_t((uint16_t) 0x0123ULL).value(1),             0ULL);
	EXPECT_EQ(uinteger_t((uint32_t) 0x01234567ULL).value(1),         0ULL);
	EXPECT_EQ(uinteger_t((uint64_t) 0x0123456789abcdefULL).value(1), 0ULL);

	EXPECT_EQ(uinteger_t((uint8_t)  0x01ULL).value(0),               (uint8_t)  0x01ULL);
	EXPECT_EQ(uinteger_t((uint16_t) 0x0123ULL).value(0),             (uint16_t) 0x0123ULL);
	EXPECT_EQ(uinteger_t((uint32_t) 0x01234567ULL).value(0),         (uint32_t) 0x01234567ULL);
	EXPECT_EQ(uinteger_t((uint64_t) 0x0123456789abcdefULL).value(0), (uint64_t) 0x0123456789abcdefULL);
}

TEST(Constructor, two) {
	for (uint8_t hi = 0; hi < 2; ++hi) {
		for (uint8_t lo = 0; lo < 2; ++lo) {
			const uinteger_t val(hi, lo);
			EXPECT_EQ(val.value(1), hi);
			EXPECT_EQ(val.value(0), lo);
		}
	}

	EXPECT_EQ(uinteger_t((uint8_t)  0x01ULL,               (uint8_t)  0x01ULL).value(1),               (uint8_t)  0x01ULL);
	EXPECT_EQ(uinteger_t((uint16_t) 0x0123ULL,             (uint16_t) 0x0123ULL).value(1),             (uint16_t) 0x0123ULL);
	EXPECT_EQ(uinteger_t((uint32_t) 0x01234567ULL,         (uint32_t) 0x01234567ULL).value(1),         (uint32_t) 0x01234567ULL);
	EXPECT_EQ(uinteger_t((uint64_t) 0x0123456789abcdefULL, (uint64_t) 0x0123456789abcdefULL).value(1), (uint64_t) 0x0123456789abcdefULL);

	EXPECT_EQ(uinteger_t((uint8_t)  0x01ULL,               (uint8_t)  0x01ULL).value(0),               (uint8_t)  0x01ULL);
	EXPECT_EQ(uinteger_t((uint16_t) 0x0123ULL,             (uint16_t) 0x0123ULL).value(0),             (uint16_t) 0x0123ULL);
	EXPECT_EQ(uinteger_t((uint32_t) 0x01234567ULL,         (uint32_t) 0x01234567ULL).value(0),         (uint32_t) 0x01234567ULL);
	EXPECT_EQ(uinteger_t((uint64_t) 0x0123456789abcdefULL, (uint64_t) 0x0123456789abcdefULL).value(0), (uint64_t) 0x0123456789abcdefULL);
}

TEST(Constructor, string) {
	EXPECT_EQ(uinteger_t("1010", 2), 10ULL);
	EXPECT_EQ(uinteger_t("033", 8), 27ULL);
	EXPECT_EQ(uinteger_t("1492", 10), 1492ULL);
	EXPECT_EQ(uinteger_t("1492"), 1492ULL);
	EXPECT_EQ(uinteger_t("000f", 16), 15ULL);

	EXPECT_EQ(uinteger_t("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 256).value(0), 0x535455565758595aULL);
	EXPECT_EQ(uinteger_t("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 256).value(1), 0x4b4c4d4e4f505152ULL);
	EXPECT_EQ(uinteger_t("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 256).value(2), 0x434445464748494aULL);
	EXPECT_EQ(uinteger_t("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 256).value(3), 0x0000000000004142ULL);
	EXPECT_EQ(uinteger_t("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 256).str(256), "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	EXPECT_EQ(uinteger_t("\330\105\140\310\23\117\21\346\241\342\64\66\73\322\155\256", 256).str(), "287473478535710989927820548052301802926");
	EXPECT_EQ(uinteger_t(std::string("\330\105\140\310\23\117\21\346\241\342\64\66\73\322\155\256", 16), 256).str(), "287473478535710989927820548052301802926");
	const char* uuid1 = "\330\105\140\310\23\117\21\346\241\342\64\66\73\322\155\256";
	EXPECT_EQ(uinteger_t(uuid1, 256).str(), "287473478535710989927820548052301802926");
	const char uuid2[] = "\330\105\140\310\23\117\21\346\241\342\64\66\73\322\155\256";
	EXPECT_EQ(uinteger_t(uuid2, 256).str(), "287473478535710989927820548052301802926");
}

TEST(Constructor, big) {
	std::random_device engine;
	std::vector<char> data(1000000);
	std::generate(begin(data), end(data), std::ref(engine));

	EXPECT_EQ(uinteger_t(data, 256).str<std::vector<char>>(256), data);
}
