/*
uint_t.hh
An unsigned integer type for C++

Copyright (c) 2017 German Mendez Bravo (Kronuz) @ german dot mb at gmail.com
Copyright (c) 2013 - 2017 Jason Lee @ calccrypto at gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

With much help from Auston Sterling

Thanks to Stefan Deigmüller for finding
a bug in operator*.

Thanks to François Dessenne for convincing me
to do a general rewrite of this class.

Germán Mández Bravo (Kronuz) converted Jason Lee's uint128_t
to header-only and extended to arbitrary bit length.
*/

#ifndef __uint_t__
#define __uint_t__

#include <cassert>
#include <vector>
#include <string>
#include <utility>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <type_traits>

// Compatibility inlines
#ifndef __has_builtin         // Optional of course
#define __has_builtin(x) 0    // Compatibility with non-clang compilers
#endif

#if defined _MSC_VER
#  define HAVE___ADDCARRY_U64
#  define HAVE___SUBBORROW_U64
#  define HAVE___UMUL128
#  include <intrin.h>
  typedef unsigned __int64 uint64_t;
#endif

#if (defined(__clang__) && __has_builtin(__builtin_clzll)) || (defined(__GNUC__ ) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)))
#  define HAVE____BUILTIN_CLZLL
#endif
#if (defined(__clang__) && __has_builtin(__builtin_addcll))
#  define HAVE____BUILTIN_ADDCLL
#endif
#if (defined(__clang__) && __has_builtin(__builtin_subcll))
#  define HAVE____BUILTIN_SUBCLL
#endif

#if defined __SIZEOF_INT128__
#define HAVE____INT64_T
#endif

class uint_t;

namespace std {  // This is probably not a good idea
	// Give uint_t type traits
	template <> struct is_arithmetic <uint_t> : std::true_type {};
	template <> struct is_integral   <uint_t> : std::true_type {};
	template <> struct is_unsigned   <uint_t> : std::true_type {};
}

class uint_t {
	public:
		static constexpr size_t karatsuba_cutoff = 70;
		static constexpr double growth_factor = 1.5;

		size_t _shifts;

		std::vector<uint64_t> _value;
		bool _carry;

		// vector window

		size_t grow(size_t n) {
			auto cc = _value.capacity();
			if (n >= cc) {
				cc = n * growth_factor;
				_value.reserve(cc);
			}
			return cc;
		}

		void prepend(size_t sz, const uint64_t& val) {
			auto min = std::min(_shifts, sz);
			if (min) {
				std::fill(_value.begin() + _shifts - min, _value.begin() + _shifts, val);
				_shifts -= min;
				sz -= min;
			}
			if (sz) {
				// _shifts should be 0 in here
				auto csz = _value.size();
				auto isz = grow(csz + sz) - csz;
				_value.insert(_value.begin(), isz, val);
				_shifts = isz - sz;
			}
		}

		void append(const uint64_t& val) {
			grow(_value.size() + 1);
			_value.push_back(val);
		}

		void append(const uint_t& val) {
			grow(_value.size() + val.end() - val.begin());
			_value.insert(_value.end(), val.begin(), val.end());
		}

		void resize(size_t sz) {
			_value.resize(sz + _shifts);
		}

		void resize(size_t sz, const uint64_t& c) {
			_value.resize(sz + _shifts, c);
		}

		void clear() {
			_value.clear();
			_shifts = 0;
		}

		uint64_t* data() noexcept {
			return _value.data() + _shifts;
		}

		const uint64_t* data() const noexcept {
			return _value.data() + _shifts;
		}

		size_t size() const noexcept {
			return _value.size() - _shifts;
		}

		std::vector<uint64_t>::iterator begin() noexcept {
			return _value.begin() + _shifts;
		}

		std::vector<uint64_t>::const_iterator begin() const noexcept {
			return _value.cbegin() + _shifts;
		}

		std::vector<uint64_t>::iterator end() noexcept {
			return _value.end();
		}

		std::vector<uint64_t>::const_iterator end() const noexcept {
			return _value.cend();
		}

		std::vector<uint64_t>::reverse_iterator rbegin() noexcept {
			return _value.rbegin();
		}

		std::vector<uint64_t>::const_reverse_iterator rbegin() const noexcept {
			return _value.crbegin();
		}

		std::vector<uint64_t>::reverse_iterator rend() noexcept {
			return std::vector<uint64_t>::reverse_iterator(_value.begin() + _shifts);
		}

		std::vector<uint64_t>::const_reverse_iterator rend() const noexcept {
			return std::vector<uint64_t>::const_reverse_iterator(_value.cbegin() + _shifts);
		}

		std::vector<uint64_t>::reference front() {
			return *begin();
		}

		std::vector<uint64_t>::const_reference front() const {
			return *begin();
		}

		std::vector<uint64_t>::reference back() {
			return *rbegin();
		}

		std::vector<uint64_t>::const_reference back() const {
			return *rbegin();
		}

		//

		template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
		void _uint_t(const T& value) {
			append(static_cast<uint64_t>(value));
		}

		template <typename T, typename... Args, typename = typename std::enable_if<std::is_integral<T>::value>::type>
		void _uint_t(const T& value, Args... args) {
			_uint_t(args...);
			append(static_cast<uint64_t>(value));
		}

		//

		static uint64_t _bits(uint64_t x) {
		#if defined HAVE____BUILTIN_CLZLL
			return x ? 64 - __builtin_clzll(x) : 1;
		#else
			uint64_t c = x ? 0 : 1;
			while (x) {
				x >>= 1;
				++c;
			}
			return c;
		#endif
		}

		static uint64_t mul(uint64_t x, uint64_t y, uint64_t* lo) {
		#if defined HAVE___UMUL128 && defined HAVE___ADDCARRY_U64
			uint64_t h;
			uint64_t l = _umul128(x, y, &h);  // _umul128(x, y, *hi) -> lo
			return h;
		#elif defined HAVE____INT64_T
			auto r = static_cast<__uint128_t>(x) * static_cast<__uint128_t>(y);
			*lo = r;
			return r >> 64;
		#else
			uint64_t x0 = x & 0xffffffffUL;
			uint64_t x1 = x >> 32;
			uint64_t y0 = y & 0xffffffffUL;
			uint64_t y1 = y >> 32;

			uint64_t u = (x0 * y0);
			uint64_t v = (x1 * y0) + (u >> 32);
			uint64_t w = (x0 * y1) + (v & 0xffffffffUL);

			*lo = (w << 32) + (u & 0xffffffffUL); // low
			return (x1 * y1) + (v >> 32) + (w >> 32); // high
		#endif
		}

		static uint64_t muladd(uint64_t x, uint64_t y, uint64_t a, uint64_t c, uint64_t* lo) {
		#if defined HAVE___UMUL128 && defined HAVE___ADDCARRY_U64
			uint64_t h;
			uint64_t l = _umul128(x, y, &h);  // _umul128(x, y, *hi) -> lo
			return h + _addcarry_u64(c, l, a, lo);  // _addcarry_u64(carryin, x, y, *sum) -> carryout
		#elif defined HAVE____INT64_T
			auto r = static_cast<__uint128_t>(x) * static_cast<__uint128_t>(y) + static_cast<__uint128_t>(a) + static_cast<__uint128_t>(c);
			*lo = r;
			return r >> 64;
		#else
			uint64_t x0 = x & 0xffffffffUL;
			uint64_t x1 = x >> 32;
			uint64_t y0 = y & 0xffffffffUL;
			uint64_t y1 = y >> 32;

			uint64_t u = (x0 * y0) + (a & 0xffffffffUL) + (c & 0xffffffffUL);
			uint64_t v = (x1 * y0) + (u >> 32) + (a >> 32) + (c >> 32);
			uint64_t w = (x0 * y1) + (v & 0xffffffffUL);

			*lo = (w << 32) + (u & 0xffffffffUL); // low
			return (x1 * y1) + (v >> 32) + (w >> 32); // high
		#endif
		}

		static uint64_t divrem(uint64_t x_hi, uint64_t x_lo, uint64_t y, uint64_t* result) {
		#if defined HAVE____INT64_T
			auto x = static_cast<__uint128_t>(x_hi) << 64 | static_cast<__uint128_t>(x_lo);
			uint64_t q = x / y;
			uint64_t r = x % y;

			*result = q;
			return r;
		#else
			// quotient
			uint64_t q = x_lo << 1;

			// remainder
			uint64_t r = x_hi;

			uint64_t carry = x_lo >> 63;
			int i;

			for (i = 0; i < 64; i++) {
				auto tmp = r >> 63;
				r <<= 1;
				r |= carry;
				carry = tmp;

				if (carry == 0) {
					if (r >= y) {
						carry = 1;
					} else {
						tmp = q >> 63;
						q <<= 1;
						q |= carry;
						carry = tmp;
						continue;
					}
				}

				r -= y;
				r -= (1 - carry);
				carry = 1;
				tmp = q >> 63;
				q <<= 1;
				q |= carry;
				carry = tmp;
			}

			*result = q;
			return r;
		#endif
		}

		static uint64_t addcarry(uint64_t x, uint64_t y, uint64_t c, uint64_t* result) {
		#if defined HAVE___ADDCARRY_U64
			return _addcarry_u64(c, x, y, result);  // _addcarry_u64(carryin, x, y, *sum) -> carryout
		#elif defined HAVE____BUILTIN_ADDCLL
			unsigned long long carryout;
			*result = __builtin_addcll(x, y, c, &carryout);  // __builtin_addcll(x, y, carryin, *carryout) -> sum
			return carryout;
		#elif defined HAVE____INT64_T
			auto r = static_cast<__uint128_t>(x) + static_cast<__uint128_t>(y) + static_cast<__uint128_t>(c);
			*result = r;
			return static_cast<bool>(r >> 64);
		#else
			uint64_t x0 = x & 0xffffffffUL;
			uint64_t x1 = x >> 32;
			uint64_t y0 = y & 0xffffffffUL;
			uint64_t y1 = y >> 32;

			auto u = x0 + y0 + c;
			auto v = x1 + y1 + static_cast<bool>(u >> 32);
			*result = (v << 32) + (u & 0xffffffffUL);
			return static_cast<bool>(v >> 32);
		#endif
		}

		static uint64_t subborrow(uint64_t x, uint64_t y, uint64_t c, uint64_t* result) {
		#if defined HAVE___SUBBORROW_U64
			return _subborrow_u64(c, x, y, result);  // _subborrow_u64(carryin, x, y, *sum) -> carryout
		#elif defined HAVE____BUILTIN_SUBCLL
			unsigned long long carryout;
			*result = __builtin_subcll(x, y, c, &carryout);  // __builtin_subcll(x, y, carryin, *carryout) -> sum
			return carryout;
		#elif defined HAVE____INT64_T
			auto r = static_cast<__uint128_t>(x) - static_cast<__uint128_t>(y) - static_cast<__uint128_t>(c);
			*result = r;
			return static_cast<bool>(r >> 64);
		#else
			uint64_t x0 = x & 0xffffffffUL;
			uint64_t x1 = x >> 32;
			uint64_t y0 = y & 0xffffffffUL;
			uint64_t y1 = y >> 32;

			auto u = x0 - y0 - c;
			auto v = x1 - y1 - static_cast<bool>(u >> 32);
			*result = (v << 32) + (u & 0xffffffffUL);
			return static_cast<bool>(v >> 32);
		#endif
		}

		void trim(uint64_t mask = 0) {
			auto rit = rbegin();
			auto rit_e = rend();

			// Masks the last value of internal vector
			mask &= 0x3f;
			if (mask && rit != rit_e) {
				*rit &= (static_cast<uint64_t>(1) << mask) - 1;
			}

			// Removes all unused zeros from the internal vector
			auto rit_f = std::find_if(rit, rit_e, [](const uint64_t& c) { return c; });
			resize(rit_e - rit_f); // shrink
		}

		int compare(const uint_t& rhs) const {
			const auto& a = size();
			const auto& b = rhs.size();
			if (a > b) return 1;
			if (a < b) return -1;
			auto rit = rbegin();
			auto rit_e = rend();
			auto rhs_rit = rhs.rbegin();
			for (; rit != rit_e; ++rit, ++rhs_rit) {
				const auto& a = *rit;
				const auto& b = *rhs_rit;
				if (a > b) return 1;
				if (a < b) return -1;
			}
			return 0;
		}

		static const uint8_t& base_bits(int base) {
			static const uint8_t _[256] = {
				0, 1, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,

				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,

				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
			};
			return _[base - 1];
		}

		static const uint8_t& base_size(int base) {
			// math.ceil(64 / math.log2(base)
			static const uint8_t _[256] = {
				0, 64, 41, 32, 28, 25, 23, 22, 21, 20, 19, 18, 18, 17, 17, 16,
				16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 13, 13,
				13, 13, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12, 12, 12,
				12, 12, 12, 12, 12, 12, 12, 12, 11, 11, 11, 11, 11, 11, 11, 11,

				11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
				11, 11, 11, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
				10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
				10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,

				10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  9,  9,  9,  9,  9,  9,
				9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
				9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
				9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,

				9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
				9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
				9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
				9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  8,
			};
			return _[base - 1];
		}

		static const uint8_t& ord(int chr) {
			// 0123456789abcdefghijklmnopqrstuvwxyz -> (0 - 35)
			static const uint8_t _[256] = {
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

				0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
				0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0xff, 0xff, 0xff, 0xff, 0xff,
				0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
				0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0xff, 0xff, 0xff, 0xff, 0xff,

				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			};
			return _[chr];
		}

		static const char& chr(int ord) {
			static const char _[256] = "0123456789abcdefghijklmnopqrstuvwxyz";
			return _[ord];
		}

		// A helper for Karatsuba multiplication to split a number in two, at n.
		static std::pair<uint_t, uint_t> karatsuba_mult_split(const uint_t& num, size_t n) {
			auto it = num.begin();
			auto it_split = it + std::min(num.size(), n);
			return std::make_pair(
				uint_t(std::vector<uint64_t>(it, it_split)),
				uint_t(std::vector<uint64_t>(it_split, num.end()))
			);
		}

		// If rhs has at least twice the digits of lhs, and lhs is big enough that
		// Karatsuba would pay off *if* the inputs had balanced sizes.
		// View rhs as a sequence of slices, each with lhs.size() digits,
		// and multiply the slices by lhs, one at a time.
		static uint_t karatsuba_lopsided_mult(const uint_t& lhs, const uint_t& rhs, size_t cutoff) {
			const auto& lhs_sz = lhs.size();
			auto rhs_sz = rhs.size();

			uint_t result;
			auto rhs_it = rhs.begin();
			size_t shift = 0;

			while (rhs_sz > 0) {
				// Multiply the next slice of rhs by lhs and add into result:
				auto slice_size = std::min(lhs_sz, rhs_sz);
				auto rhs_end = uint_t(std::vector<uint64_t>(rhs_it, rhs_it + slice_size));
				auto product = karatsuba_mult(lhs, rhs_end, cutoff);
				result.add(product, shift);
				shift += slice_size;
				rhs_sz -= slice_size;
				rhs_it += slice_size;
			}

			return result;
		}

		uint_t(const std::vector<uint64_t>& value) :
			_shifts(0),
			_value(value),
			_carry(false) {
			trim();
		}

	public:
		// Constructors
		uint_t() :
			_shifts(0),
			_carry(false) { }

		uint_t(const uint_t& o) :
			_shifts(o._shifts),
			_value(o._value),
			_carry(o._carry) { }

		uint_t(uint_t&& o) :
			_shifts(std::move(o._shifts)),
			_value(std::move(o._value)),
			_carry(std::move(o._carry)) { }

		template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
		uint_t(const T& value) :
			_shifts(0),
			_carry(false) {
			if (value) {
				append(static_cast<uint64_t>(value));
			}
		}

		template <typename T, typename... Args, typename = typename std::enable_if<std::is_integral<T>::value>::type>
		uint_t(const T& value, Args... args) :
			_shifts(0),
			_carry(false) {
			_uint_t(args...);
			append(static_cast<uint64_t>(value));
			trim();
		}

		explicit uint_t(const char* bytes, size_t sz, int base) :
			uint_t(strtouint(bytes, sz, base)) { }

		template <typename T, size_t N>
		explicit uint_t(T (&s)[N], int base=10) :
			uint_t(s, N - 1, base) { }

		template <typename T>
		explicit uint_t(const std::vector<T>& bytes, int base=10) :
			uint_t(bytes.data(), bytes.size(), base) { }

		explicit uint_t(const std::string& bytes, int base=10) :
			uint_t(bytes.data(), bytes.size(), base) { }

		//  RHS input args only

		// Assignment Operator
		uint_t& operator=(const uint_t& o) {
			_shifts = o._shifts;
			_value = o._value;
			_carry = o._carry;
			return *this;
		}
		uint_t& operator=(uint_t&& o) {
			_shifts = std::move(o._shifts);
			_value = std::move(o._value);
			_carry = std::move(o._carry);
			return *this;
		}

		// Typecast Operators
		explicit operator bool() const {
			return static_cast<bool>(size());
		}
		explicit operator unsigned char() const {
			return static_cast<unsigned char>(size() ? front() : 0);
		}
		explicit operator unsigned short() const {
			return static_cast<unsigned short>(size() ? front() : 0);
		}
		explicit operator unsigned int() const {
			return static_cast<unsigned int>(size() ? front() : 0);
		}
		explicit operator unsigned long() const {
			return static_cast<unsigned long>(size() ? front() : 0);
		}
		explicit operator unsigned long long() const {
			return static_cast<unsigned long long>(size() ? front() : 0);
		}
		explicit operator char() const {
			return static_cast<char>(size() ? front() : 0);
		}
		explicit operator short() const {
			return static_cast<short>(size() ? front() : 0);
		}
		explicit operator int() const {
			return static_cast<int>(size() ? front() : 0);
		}
		explicit operator long() const {
			return static_cast<long>(size() ? front() : 0);
		}
		explicit operator long long() const {
			return static_cast<long long>(size() ? front() : 0);
		}

		// Bitwise Operators
		uint_t operator&(const uint_t& rhs) const {
			uint_t result(*this);
			result &= rhs;
			return result;
		}

		uint_t& operator&=(const uint_t& rhs) {
			if (size() > rhs.size()) {
				resize(rhs.size()); // shrink
			}
			auto it = begin();
			auto it_e = end();
			auto rhs_it = rhs.begin();
			for (; it != it_e; ++it, ++rhs_it) {
				*it &= *rhs_it;
			}
			trim();
			return *this;
		}

		uint_t operator|(const uint_t& rhs) const {
			uint_t result(*this);
			result |= rhs;
			return result;
		}

		uint_t& operator|=(const uint_t& rhs) {
			if (size() < rhs.size()) {
				resize(rhs.size(), 0); // grow
			}
			auto it = begin();
			auto rhs_it = rhs.begin();
			auto rhs_it_e = rhs.end();
			for (; rhs_it != rhs_it_e; ++it, ++rhs_it) {
				*it |= *rhs_it;
			}
			trim();
			return *this;
		}

		uint_t operator^(const uint_t& rhs) const {
			uint_t result(*this);
			result ^= rhs;
			return result;
		}

		uint_t& operator^=(const uint_t& rhs) {
			if (size() < rhs.size()) {
				resize(rhs.size(), 0); // grow
			}
			auto it = begin();
			auto rhs_it = rhs.begin();
			auto rhs_it_e = rhs.end();
			for (; rhs_it != rhs_it_e; ++it, ++rhs_it) {
				*it ^= *rhs_it;
			}
			trim();
			return *this;
		}

		uint_t& inv() {
			if (!size()) {
				append(0);
			}
			auto b = bits();
			auto it = begin();
			auto it_e = end();
			for (; it != it_e; ++it) {
				*it = ~*it;
			}
			trim(b);
			return *this;
		}

		uint_t operator~() const {
			uint_t result(*this);
			result.inv();
			return result;
		}

		// Bit Shift Operators
		uint_t operator<<(const uint_t& rhs) const {
			uint_t result(*this);
			result <<= rhs;
			return result;
		}

		uint_t& operator<<=(const uint_t& rhs) {
			if (rhs == 0) {
				return *this;
			}
			assert(rhs.size() == 1);
			auto shift = rhs.front();
			auto shifts = shift / 64;
			shift = shift % 64;
			if (shift) {
				uint64_t shifted = 0;
				auto it = begin();
				auto it_e = end();
				for (; it != it_e; ++it) {
					auto v = (*it << shift) | shifted;
					shifted = *it >> (64 - shift);
					*it = v;
				}
				if (shifted) {
					append(shifted);
				}
			}
			if (shifts) {
				prepend(shifts, 0);
			}
			return *this;
		}

		uint_t operator>>(const uint_t& rhs) const {
			uint_t result(*this);
			result >>= rhs;
			return result;
		}

		uint_t& operator>>=(const uint_t& rhs) {
			if (rhs >= size() * 64) {
				clear();
				return *this;
			} else if (rhs == 0) {
				return *this;
			}
			assert(rhs.size() == 1);
			auto shift = rhs.front();
			auto shifts = shift / 64;
			shift = shift % 64;
			if (shifts) {
				_shifts += shifts;
			}
			if (shift) {
				uint64_t shifted = 0;
				auto rit = rbegin();
				auto rit_e = rend();
				for (; rit != rit_e; ++rit) {
					auto v = (*rit >> shift) | shifted;
					shifted = *rit << (64 - shift);
					*rit = v;
				}
				trim();
			}
			return *this;
		}

		// Logical Operators
		bool operator!() const {
			return !static_cast<bool>(*this);
		}

		bool operator&&(const uint_t& rhs) const {
			return static_cast<bool>(*this) && rhs;
		}

		bool operator||(const uint_t& rhs) const {
			return static_cast<bool>(*this) || rhs;
		}

		// Comparison Operators
		bool operator==(const uint_t& rhs) const {
			return compare(rhs) == 0;
		}

		bool operator!=(const uint_t& rhs) const {
			return compare(rhs) != 0;
		}

		bool operator>(const uint_t& rhs) const {
			return compare(rhs) > 0;
		}

		bool operator<(const uint_t& rhs) const {
			return compare(rhs) < 0;
		}

		bool operator>=(const uint_t& rhs) const {
			return compare(rhs) >= 0;
		}

		bool operator<=(const uint_t& rhs) const {
			return compare(rhs) <= 0;
		}

		// Arithmetic Operators
		uint_t operator+(const uint_t& rhs) const {
			uint_t result(*this);
			result += rhs;
			return result;
		}

		uint_t& add(const uint_t& rhs, size_t shift=0) {
			// First try saving some calculations:
			if (!rhs) {
				return *this;
			}
			const auto& sz = size();
			const auto& rhs_sz = rhs.size();
			if (shift) {
				shift = std::min(shift, sz);
			}
			if (sz < rhs_sz + shift) {
				resize(rhs_sz + shift, 0); // grow
			}
			auto it = begin() + shift;
			auto it_e = end();
			auto rhs_it = rhs.begin();
			auto rhs_it_e = rhs.end();
			uint64_t carry = 0;
			for (; it != it_e && rhs_it != rhs_it_e; ++it, ++rhs_it) {
				carry = addcarry(*it, *rhs_it, carry, &*it);
			}
			for (; carry && it != it_e; ++it) {
				carry = addcarry(*it, 0, carry, &*it);
			}
			if (carry) {
				append(1);
			}
			_carry = false;
			trim();
			return *this;
		}

		uint_t& operator+=(const uint_t& rhs) {
			return add(rhs, 0);
		}

		uint_t operator-(const uint_t& rhs) const {
			uint_t result(*this);
			result -= rhs;
			return result;
		}

		uint_t& sub(const uint_t& rhs, size_t shift=0) {
			// First try saving some calculations:
			if (!rhs) {
				return *this;
			}
			const auto& sz = size();
			const auto& rhs_sz = rhs.size();
			if (shift) {
				shift = std::min(shift, sz);
			}
			if (sz < rhs_sz + shift) {
				resize(rhs_sz + shift, 0); // grow
			}
			auto it = begin() + shift;
			auto it_e = end();
			auto rhs_it = rhs.begin();
			auto rhs_it_e = rhs.end();
			uint64_t carry = 0;
			for (; it != it_e && rhs_it != rhs_it_e; ++it, ++rhs_it) {
				carry = subborrow(*it, *rhs_it, carry, &*it);
			}
			for (; carry && it != it_e; ++it) {
				carry = subborrow(*it, 0, carry, &*it);
			}
			_carry = carry;

			trim();
			return *this;
		}

		uint_t& operator-=(const uint_t& rhs) {
			return sub(rhs, 0);
		}

		// Long multiplication
		static uint_t long_mult(const uint_t& lhs, const uint_t& rhs) {
			const auto& lhs_sz = lhs.size();
			const auto& rhs_sz = rhs.size();

			if (lhs_sz > rhs_sz) {
				// rhs should be the largest:
				return long_mult(rhs, lhs);
			}

			uint_t result;
			result.resize(rhs.size() + lhs.size(), 0);

			auto it_lhs = lhs.begin();
			auto it_lhs_e = lhs.end();

			auto it_rhs = rhs.begin();
			auto it_rhs_e = rhs.end();

			auto it_result = result.begin();
			auto it_result_s = it_result;
			auto it_result_l = it_result;

			for (; it_lhs != it_lhs_e; ++it_lhs, ++it_result) {
				if (auto lhs_it_val = *it_lhs) {
					auto _it_rhs = it_rhs;
					auto _it_result = it_result;
					uint64_t carry = 0;
					for (; _it_rhs != it_rhs_e; ++_it_rhs, ++_it_result) {
						carry = muladd(*_it_rhs, lhs_it_val, *_it_result, carry, &*_it_result);
					}
					if (carry) {
						*_it_result++ = carry;
					}
					if (it_result_l < _it_result) {
						it_result_l = _it_result;
					}
				}
			}

			result.resize(it_result_l - it_result_s); // shrink

			// Finish up
			result.trim();
			return result;
		}

		// Karatsuba multiplication
		static uint_t karatsuba_mult(const uint_t& lhs, const uint_t& rhs, size_t cutoff = 1) {
			const auto& lhs_sz = lhs.size();
			const auto& rhs_sz = rhs.size();

			if (lhs_sz > rhs_sz) {
				// rhs should be the largest:
				return karatsuba_mult(rhs, lhs, cutoff);
			}

			if (lhs_sz <= cutoff) {
				return long_mult(lhs, rhs);
			}

			// If a is too small compared to b, splitting on b gives a degenerate case
			// in which Karatsuba may be (even much) less efficient than long multiplication.
			if (2 * lhs_sz <= rhs_sz) {
				return karatsuba_lopsided_mult(lhs, rhs, cutoff);
			}

			// Karatsuba:
			//
			//                  A      B
			//               x  C      D
			//     ---------------------
			//                 AD     BD
			//       AC        BC
			//     ---------------------
			//       AC    AD + BC    BD
			//
			//  AD + BC  =
			//  AC + AD + BC + BD - AC - BD
			//  (A + B) (C + D) - AC - BD

			// Calculate the split point near the middle of the largest (rhs).
			auto shift = rhs_sz >> 1;

			// Split to get A and B:
			auto lhs_pair = karatsuba_mult_split(lhs, shift);
			auto& A = lhs_pair.second; // hi
			auto& B = lhs_pair.first;  // lo

			// Split to get C and D:
			auto rhs_pair = karatsuba_mult_split(rhs, shift);
			auto& C = rhs_pair.second; // hi
			auto& D = rhs_pair.first;  // lo

			// Get the pieces:
			auto AC = karatsuba_mult(A, C, cutoff);
			auto BD = karatsuba_mult(B, D, cutoff);
			auto AD_BC = karatsuba_mult((A + B), (C + D), cutoff) - AC - BD;

			// Join the pieces, AC and BD (can't overlap) into BD:
			BD.grow(shift * 2 + AC.size());
			BD.resize(shift * 2, 0);
			BD.append(AC);

			// And add AD_BC to the middle: (AC           BD) + (    AD + BC    ):
			BD.add(AD_BC, shift);

			// Finish up
			BD.trim();
			return BD;
		}

		static uint_t mult(const uint_t& lhs, const uint_t& rhs) {
			// First try saving some calculations:
			if (!lhs || !rhs) {
				return uint_0();
			}
			if (lhs == uint_1()) {
				return rhs;
			}
			if (rhs == uint_1()) {
				return lhs;
			}

			return karatsuba_mult(lhs, rhs, karatsuba_cutoff);
		}

		uint_t operator*(const uint_t& rhs) const {
			return mult(*this, rhs);
		}

		uint_t& operator*=(const uint_t& rhs) {
			*this = *this * rhs;
			return *this;
		}

		static const uint_t uint_0() {
			static uint_t uint_0(0);
			return uint_0;
		}

		static const uint_t uint_1() {
			static uint_t uint_1(1);
			return uint_1;
		}

		// Single word division
		// Fastests, but ONLY for single sized rhs
		static std::pair<uint_t, uint_t> single_divmod(const uint_t& lhs, const uint_t& rhs) {
			assert(rhs.size() == 1);
			auto n = rhs.front();

			auto rit_lhs = lhs.rbegin();
			auto rit_lhs_e = lhs.rend();

			auto q = uint_0();
			q.resize(lhs.size(), 0);
			auto rit_q = q.rbegin();

			uint64_t r = 0;
			for (; rit_lhs != rit_lhs_e; ++rit_lhs, ++rit_q) {
				r = divrem(r, *rit_lhs, n, &*rit_q);
			}

			q.trim();

			return std::make_pair(q, uint_t(r));
		}

		// Implementation of Knuth's Algorithm D
		static std::pair<uint_t, uint_t> knuth_divmod(const uint_t& lhs, const uint_t& rhs) {
			auto v = lhs;
			auto w = rhs;

			auto v_size = v.size();
			auto w_size = w.size();
			assert(v_size >= w_size && w_size >= 2);

			// D1. normalize: shift rhs left so that its top digit is >= 63 bits.
			// shift lhs left by the same amount. Results go into w and v.
			auto d = 64 - _bits(w.back());
			v <<= d;
			w <<= d;

			if (*v.rbegin() >= *w.rbegin()) {
				v.append(0);
			}
			v_size = v.size();
			v.append(0);

			// Now *v.rbegin() < *w.rbegin() so quotient has at most
			// (and usually exactly) k = v.size() - w.size() digits.
			auto k = v_size - w_size;
			auto q = uint_0();
			q.resize(k + 1, 0);

			auto rit_q = q.rend() - (k + 1);

			auto it_v_b = v.begin();
			auto it_v_k = it_v_b + k;

			auto it_w = w.begin();
			auto it_w_e = w.end();

			auto rit_w = w.rbegin();
			auto wm1 = *rit_w++;
			auto wm2 = *rit_w;

			// D2. inner loop: divide v[k+0..k+n] by w[0..n]
			for (; it_v_k >= it_v_b; --it_v_k, ++rit_q) {
				// D3. Compute estimate quotient digit q; may overestimate by 1 (rare)
				uint64_t _q;
				auto _r = divrem(*(it_v_k + w_size), *(it_v_k + w_size - 1), wm1, &_q);
				uint64_t mullo = 0;
				auto mulhi = mul(wm2, _q, &mullo);
				auto rlo = *(it_v_k + w_size - 2);
				while (mulhi > _r || (mulhi == _r && mullo > rlo)) {
					--_q;
					auto _r2 = _r + wm1;
					if (_r2 < _r) break;
					_r = _r2;
					mulhi = mul(wm2, _q, &mullo);
				}

				// D4. Multiply and subtract _q * w0[0:size_w] from vk[0:size_w+1]
				auto _it_v = it_v_k;
				auto _it_w = it_w;
				mulhi = 0;
				uint64_t carry = 0;
				for (; _it_w != it_w_e; ++_it_v, ++_it_w) {
					uint64_t mullo = 0;
					mulhi = muladd(*_it_w, _q, 0, mulhi, &mullo);
					carry = subborrow(*_it_v, mullo, carry, &*_it_v);
				}
				carry = subborrow(*_it_v, 0, carry, &*_it_v);

				if (carry) {
					// D6. Add w back if q was too large (this branch taken rarely)
					--_q;

					auto _it_v = it_v_k;
					auto _it_w = it_w;
					carry = 0;
					for (; _it_w != it_w_e; ++_it_v, ++_it_w) {
						carry = addcarry(*_it_v, *_it_w, carry, &*_it_v);
					}
					carry = addcarry(*_it_v, 0, carry, &*_it_v);
				}

				/* store quotient digit */
				*rit_q = _q;
			}

			// D8. unnormalize: unshift remainder.
			v.resize(w_size);
			v >>= d;

			q.trim();
			v.trim();

			return std::make_pair(q, v);
		}

		static std::pair<uint_t, uint_t> divmod(const uint_t& lhs, const uint_t& rhs) {
			// First try saving some calculations:
			if (!rhs) {
				throw std::domain_error("Error: division or modulus by 0");
			}
			if (lhs.size() - rhs.size() == 0) {
				// Fast division and modulo for single value
				const auto& a = *lhs.begin();
				const auto& b = *rhs.begin();
				return std::make_pair(uint_t(a / b), uint_t(a % b));
			}
			if (rhs == uint_1()) {
				return std::make_pair(lhs, uint_0());
			}
			if (lhs == rhs) {
				return std::make_pair(uint_1(), uint_0());
			}
			if (!lhs || lhs < rhs) {
				return std::make_pair(uint_0(), lhs);
			}
			if (rhs.size() == 1) {
				return single_divmod(lhs, rhs);
			}

			return knuth_divmod(lhs, rhs);
		}

		uint_t operator/(const uint_t& rhs) const {
			return divmod(*this, rhs).first;
		}

		uint_t& operator/=(const uint_t& rhs) {
			*this = *this / rhs;
			return *this;
		}

		uint_t operator%(const uint_t& rhs) const {
			return divmod(*this, rhs).second;
		}

		uint_t& operator%=(const uint_t& rhs) {
			*this = *this % rhs;
			return *this;
		}

		// Increment Operator
		uint_t& operator++() {
			return *this += uint_1();
		}
		uint_t operator++(int) {
			uint_t temp(*this);
			++*this;
			return temp;
		}

		// Decrement Operator
		uint_t& operator--() {
			return *this -= uint_1();
		}
		uint_t operator--(int) {
			uint_t temp(*this);
			--*this;
			return temp;
		}

		// Nothing done since promotion doesn't work here
		uint_t operator+() const {
			return *this;
		}

		// two's complement
		uint_t operator-() const {
			return uint_0() - *this;
		}

		// Get private value at index
		const uint64_t& value(size_t idx) const {
			static const uint64_t zero = 0;
			return idx < size() ? *(begin() + idx) : zero;
		}

		// Get value of bit N
		bool operator[](size_t n) const {
			auto nd = n / 64;
			auto nm = n % 64;
			return nd < size() ? (*(begin() + nd) >> nm) & 1 : 0;
		}

		// Get bitsize of value
		size_t bits() const {
			size_t out = 0;
			if (size()) {
				out = (size() - 1) * 64;
				uint64_t ms = back();
				out += _bits(ms);
			}
			return out;
		}

		static uint_t strtouint(const char* bytes, size_t sz, int base) {
			uint_t result;

			if (base >= 2 && base <= 36) {
				uint_t bits = base_bits(base);
				if (bits) {
					for (; sz; --sz, ++bytes) {
						auto d = ord(static_cast<int>(*bytes));
						if (d >= base) {
							throw std::runtime_error("Error: Not a digit in base " + std::to_string(base) + ": '" + std::string(1, *bytes) + "'");
						}
						result = (result << bits) | d;
					}
				} else {
					for (; sz; --sz, ++bytes) {
						auto d = ord(static_cast<int>(*bytes));
						if (d >= base) {
							throw std::runtime_error("Error: Not a digit in base " + std::to_string(base) + ": '" + std::string(1, *bytes) + "'");
						}
						result = (result * base) + d;
					}
				}
			} else if (sz && base == 256) {
				auto value_size = sz / sizeof(uint64_t);
				auto value_padding = sz % sizeof(uint64_t);
				if (value_padding) {
					value_padding = sizeof(uint64_t) - value_padding;
					++value_size;
				}
				result.resize(value_size); // grow (no initialization)
				*result.begin() = 0; // initialize value
				auto ptr = reinterpret_cast<char*>(result.data());
				std::copy(bytes, bytes + sz, ptr + value_padding);
				std::reverse(ptr, ptr + value_size * sizeof(uint64_t));
			} else {
				throw std::runtime_error("Error: Cannot convert from base " + std::to_string(base));
			}

			return result;
		}

		// Get string representation of value
		template <typename Result = std::string>
		Result str(int base = 10) const {
			if (base >= 2 && base <= 36) {
				Result result;
				if (size()) {
					auto bits = base_bits(base);
					result.reserve(size() * base_size(base));
					if (bits) {
						uint64_t mask = base - 1;
						auto shift = 0;
						auto ptr = reinterpret_cast<const uint32_t*>(data());
						uint64_t num = *ptr++;
						num <<= 32;
						for (auto i = size() * 2 - 1; i; --i) {
							num >>= 32;
							num |= (static_cast<uint64_t>(*ptr++) << 32);
							do {
								result.push_back(chr(static_cast<int>((num >> shift) & mask)));
								shift += bits;
							} while (shift <= 32);
							shift -= 32;
						}
						num >>= (shift + 32);
						while (num) {
							result.push_back(chr(static_cast<int>(num & mask)));
							num >>= bits;
						}
						auto rit_f = std::find_if(result.rbegin(), result.rend(), [](const char& c) { return c != '0'; });
						result.resize(result.rend() - rit_f); // shrink
					} else {
						std::pair<uint_t, uint_t> qr(*this, uint_0());
						do {
							qr = divmod(qr.first, base);
							result.push_back(chr(static_cast<int>(qr.second)));
						} while (qr.first);
					}
					std::reverse(result.begin(), result.end());
				} else {
					result.push_back('0');
				}
				return result;
			} else if (base == 256) {
				if (size()) {
					auto ptr = reinterpret_cast<const char*>(data());
					Result result(ptr, ptr + size() * sizeof(uint64_t));
					auto rit_f = std::find_if(result.rbegin(), result.rend(), [](const char& c) { return c; });
					result.resize(result.rend() - rit_f); // shrink
					std::reverse(result.begin(), result.end());
					return result;
				} else {
					Result result;
					result.push_back('\x00');
					return result;
				}
			} else {
				throw std::invalid_argument("Base must be in the range [2, 36]");
			}
		}

		template <typename Result = std::string>
		Result bin() const {
			return str<Result>(2);
		}

		template <typename Result = std::string>
		Result oct() const {
			return str<Result>(8);
		}

		template <typename Result = std::string>
		Result hex() const {
			return str<Result>(16);
		}

		template <typename Result = std::string>
		Result raw() const {
			return str<Result>(256);
		}
};

namespace std {  // This is probably not a good idea
	// Make it work with std::string()
	inline std::string to_string(uint_t& num) {
		return num.str();
	}
	inline const std::string to_string(const uint_t& num) {
		return num.str();
	}
};

// lhs type T as first arguemnt
// If the output is not a bool, casts to type T

// Bitwise Operators
template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
uint_t operator&(const T& lhs, const uint_t& rhs) {
	return rhs & lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator&=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(rhs & lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
uint_t operator|(const T& lhs, const uint_t& rhs) {
	return rhs | lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator|=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(rhs | lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
uint_t operator^(const T& lhs, const uint_t& rhs) {
	return rhs ^ lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator^=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(rhs ^ lhs);
}

// Bitshift operators
inline uint_t operator<<(const bool     & lhs, const uint_t& rhs) {
	return uint_t(lhs) << rhs;
}
inline uint_t operator<<(const uint8_t  & lhs, const uint_t& rhs) {
	return uint_t(lhs) << rhs;
}
inline uint_t operator<<(const uint16_t & lhs, const uint_t& rhs) {
	return uint_t(lhs) << rhs;
}
inline uint_t operator<<(const uint32_t & lhs, const uint_t& rhs) {
	return uint_t(lhs) << rhs;
}
inline uint_t operator<<(const uint64_t & lhs, const uint_t& rhs) {
	return uint_t(lhs) << rhs;
}
inline uint_t operator<<(const int8_t   & lhs, const uint_t& rhs) {
	return uint_t(lhs) << rhs;
}
inline uint_t operator<<(const int16_t  & lhs, const uint_t& rhs) {
	return uint_t(lhs) << rhs;
}
inline uint_t operator<<(const int32_t  & lhs, const uint_t& rhs) {
	return uint_t(lhs) << rhs;
}
inline uint_t operator<<(const int64_t  & lhs, const uint_t& rhs) {
	return uint_t(lhs) << rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator<<=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(uint_t(lhs) << rhs);
}

inline uint_t operator>>(const bool     & lhs, const uint_t& rhs) {
	return uint_t(lhs) >> rhs;
}
inline uint_t operator>>(const uint8_t  & lhs, const uint_t& rhs) {
	return uint_t(lhs) >> rhs;
}
inline uint_t operator>>(const uint16_t & lhs, const uint_t& rhs) {
	return uint_t(lhs) >> rhs;
}
inline uint_t operator>>(const uint32_t & lhs, const uint_t& rhs) {
	return uint_t(lhs) >> rhs;
}
inline uint_t operator>>(const uint64_t & lhs, const uint_t& rhs) {
	return uint_t(lhs) >> rhs;
}
inline uint_t operator>>(const int8_t   & lhs, const uint_t& rhs) {
	return uint_t(lhs) >> rhs;
}
inline uint_t operator>>(const int16_t  & lhs, const uint_t& rhs) {
	return uint_t(lhs) >> rhs;
}
inline uint_t operator>>(const int32_t  & lhs, const uint_t& rhs) {
	return uint_t(lhs) >> rhs;
}
inline uint_t operator>>(const int64_t  & lhs, const uint_t& rhs) {
	return uint_t(lhs) >> rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator>>=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(uint_t(lhs) >> rhs);
}

// Comparison Operators
template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool operator==(const T& lhs, const uint_t& rhs) {
	return rhs == lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool operator!=(const T& lhs, const uint_t& rhs) {
	return rhs != lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool operator>(const T& lhs, const uint_t& rhs) {
	return rhs < lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool operator<(const T& lhs, const uint_t& rhs) {
	return rhs > lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool operator>=(const T& lhs, const uint_t& rhs) {
	return rhs <= lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool operator<=(const T& lhs, const uint_t& rhs) {
	return rhs >= lhs;
}

// Arithmetic Operators
template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
uint_t operator+(const T& lhs, const uint_t& rhs) {
	return rhs + lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator+=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(rhs + lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
uint_t operator-(const T& lhs, const uint_t& rhs) {
	return -(rhs - lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator-=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(-(rhs - lhs));
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
uint_t operator*(const T& lhs, const uint_t& rhs) {
	return rhs * lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator*=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(rhs * lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
uint_t operator/(const T& lhs, const uint_t& rhs) {
	return uint_t(lhs) / rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator/=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(uint_t(lhs) / rhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
uint_t operator%(const T& lhs, const uint_t& rhs) {
	return uint_t(lhs) % rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T& operator%=(T& lhs, const uint_t& rhs) {
	return lhs = static_cast<T>(uint_t(lhs) % rhs);
}

// IO Operator
inline std::ostream& operator<<(std::ostream& stream, const uint_t& rhs) {
	if (stream.flags() & stream.oct) {
		stream << rhs.str(8);
	} else if (stream.flags() & stream.dec) {
		stream << rhs.str(10);
	} else if (stream.flags() & stream.hex) {
		stream << rhs.str(16);
	}
	return stream;
}

#endif
