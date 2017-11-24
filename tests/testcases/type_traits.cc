#include <type_traits>

#include <gtest/gtest.h>

#include "uinteger_t.hh"

TEST(Type_Traits, is_arithmetic) {
	EXPECT_EQ(std::is_arithmetic <uinteger_t>::value, true);
}

TEST(Type_Traits, is_integral) {
	EXPECT_EQ(std::is_integral <uinteger_t>::value, true);
}

TEST(Type_Traits, is_unsigned) {
	EXPECT_EQ(std::is_unsigned <uinteger_t>::value, true);
}
