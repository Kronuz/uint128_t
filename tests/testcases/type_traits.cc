#include <type_traits>

#include <gtest/gtest.h>

#include "uint_t.hh"

TEST(Type_Traits, is_arithmetic) {
	EXPECT_EQ(std::is_arithmetic <uint_t>::value, true);
}

TEST(Type_Traits, is_integral) {
	EXPECT_EQ(std::is_integral <uint_t>::value, true);
}

TEST(Type_Traits, is_unsigned) {
	EXPECT_EQ(std::is_unsigned <uint_t>::value, true);
}
