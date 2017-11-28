# uinteger_t [![License][license-img]][license-url] [![GitHub Stars][stars-img]][stars-url] [![GitHub Forks][forks-img]][forks-url] [![GitHub Watchers][watchers-img]][watchers-url] [![Tweet][tweet-img]][tweet-url]

[![Build Status](https://travis-ci.org/Kronuz/uinteger_t.svg?branch=master)](https://travis-ci.org/Kronuz/uinteger_t)


### An arbitrary precision unsigned integer type for C++

This is a header-only implementation of an arbitrary precision unsigned integer
type in modern C++. It's meant to be used like a standard unsigned integer,
except operations can use really big numbers and can take multiple base strings
as inputs.


#### Example

``` cpp
// example.cc
// g++ -std=c++14 -o example example.cc

#include <iostream>
#include "uinteger_t.hh"

int main() {
    uinteger_t a = 3735879680;
    uinteger_t b("beee", 16);  // string input (no base prefixes: '0b' or '0x')
    uinteger_t num = a + b + 1;

    std::cout << num             << std::endl;  // print directly
    // => 3735928559

    std::cout << num.str(2)     << std::endl;  // print as a string on a specific base
    // => 11011110101011011011111011101111

    std::cout << std::hex << num << std::endl;  // print using std::oct, std::dec, and std::hex
    // => deadbeef

    return 0;
}
```


#### Compilation

* g++ and clang++ are supported.
* C++14 is required.


## Internals

Data is stored in a `std::vector<uint64_t>` in little-endian form, so `_value[0]`
is the least significant digit. Operations are optimized for fast performance:

* Addition and subtraction use regular (optimized) 64-bit operations with carry/borrow.

* Shifts try to grow vector in the most efficient way, using a two direction growth
  factor of 1.5.

* Multiplication uses long multiplication for numbers < 1024 bits and uses Karatsuba
  (and lopsided Karatsuba) for bigger numbers with a no-copying approach.

* Division and modulus use long division from Knuth's Algorithm D.


## Author
[**German Mendez Bravo (Kronuz)**](https://kronuz.io/)

[![Follow on GitHub][github-follow-img]][github-follow-url]
[![Follow on Twitter][twitter-follow-img]][twitter-follow-url]

Based upon Jason Lee's uint128_t module and modified by Kronuz to
support an arbitrary number of bits.


## License

MIT License. See [LICENSE](LICENSE) for details.

Copyright (c) 2017 German Mendez Bravo (Kronuz) @ german dot mb at gmail.com

Copyright (c) 2013 - 2017 Jason Lee @ calccrypto at gmail.com


[license-url]: https://github.com/Kronuz/uinteger_t/blob/master/LICENSE
[license-img]: https://img.shields.io/github/license/Kronuz/uinteger_t.svg
[stars-url]: https://github.com/Kronuz/uinteger_t/stargazers
[stars-img]: https://img.shields.io/github/stars/Kronuz/uinteger_t.svg?style=social&amp;label=Stars
[forks-url]: https://github.com/Kronuz/uinteger_t/network/members
[forks-img]: https://img.shields.io/github/forks/Kronuz/uinteger_t.svg?style=social&amp;label=Forks
[watchers-url]: https://github.com/Kronuz/uinteger_t/watchers
[watchers-img]: https://img.shields.io/github/watchers/Kronuz/uinteger_t.svg?style=social&amp;label=Watchers
[tweet-img]: https://img.shields.io/twitter/url/https/github.com/Kronuz/uinteger_t.svg?style=social
[tweet-url]: https://twitter.com/intent/tweet?text=An+unsigned+integer+type+for+C%2B%2B+by%2B%40germbravo:&url=https%3A%2F%2Fgithub.com%2FKronuz%2Fuint_t
[github-follow-url]: https://github.com/Kronuz
[github-follow-img]: https://img.shields.io/github/followers/Kronuz.svg?style=social&label=Follow
[twitter-follow-url]: https://twitter.com/intent/follow?screen_name=germbravo
[twitter-follow-img]: https://img.shields.io/twitter/follow/germbravo.svg?style=social&label=Follow
