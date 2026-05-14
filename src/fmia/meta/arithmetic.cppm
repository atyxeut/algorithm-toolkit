// Copyright (C) 2026 atyxeut
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <https://www.gnu.org/licenses/>.

module;

#include <climits>

export module fmia.meta.arithmetic;

import std;

import fmia.math.integer.fixed_precision;
import fmia.math.integer.arbitrary_precision;
import fmia.math.floating_point.ieee754;
import fmia.math.floating_point.arbitrary_precision;
import fmia.meta.cv_qualifier;

export namespace fmia::meta {

template <typename T>
concept fixed_precision_arithmetic = fixed_precision_integral<T> || ieee754_floating_point<T>;

template <typename T>
concept arbitratry_precision_arithmetic = arbitrary_precision_integral<T> || arbitrary_precision_floating_point<T>;

template <typename T>
concept arithmetic = integral<T> || floating_point<T>;

template <typename T>
concept nothrow_arithmetic = nothrow_integral<T> || nothrow_floating_point<T>;

} // export namespace fmia::meta

export namespace fmia::meta {

template <typename T>
constexpr usize precision_bits_v = sizeof(T) * CHAR_BIT;

template <>
constexpr usize precision_bits_v<i128> = 128;

template <>
constexpr usize precision_bits_v<u128> = 128;

template <>
constexpr usize precision_bits_v<f128> = 128;

template <usize Bits>
constexpr usize precision_bits_v<integer::i<Bits>> = Bits;

template <usize Bits>
constexpr usize precision_bits_v<integer::u<Bits>> = Bits;

template <usize Bits>
constexpr usize precision_bits_v<ieee754::f<Bits>> = Bits;

template <usize Bits>
constexpr usize precision_bits_v<ieee754::d<Bits>> = Bits;

template <typename T, typename U>
concept precision_comparable =
  (twos_complement_signed_integral<T> && twos_complement_signed_integral<U>)
  || (unsigned_integral<T> && unsigned_integral<U>)
  || (ieee754_binary_floating_point<T> && ieee754_binary_floating_point<U>)
  || (ieee754_decimal_floating_point<T> && ieee754_decimal_floating_point<U>);

template <typename T, typename U>
  requires precision_comparable<T, U>
struct compare_precision
{
  static constexpr int value = [] {
    constexpr auto x = precision_bits_v<T>, y = precision_bits_v<U>;
    return (x > y) - (x < y);
  }();
};

template <typename T, typename U>
  requires precision_comparable<T, U>
constexpr int compare_precision_v = compare_precision<T, U>::value;

template <typename T, typename U>
concept precision_eq = compare_precision_v<T, U> == 0;

template <typename T, typename U>
concept precision_neq = compare_precision_v<T, U> != 0;

template <typename T, typename U>
concept precision_lt = compare_precision_v<T, U> < 0;

template <typename T, typename U>
concept precision_lteq = compare_precision_v<T, U> <= 0;

template <typename T, typename U>
concept precision_gt = compare_precision_v<T, U> > 0;

template <typename T, typename U>
concept precision_gteq = compare_precision_v<T, U> >= 0;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename, usize>
struct make_higher_precision_selector_for_standard_integral_impl;

template <typename T>
struct make_higher_precision_selector_for_standard_integral_impl<T, 0>
  : std::conditional<std::signed_integral<T>, claim_cv_t<T, i32>, claim_cv_t<T, u32>>
{
};

template <typename T>
struct make_higher_precision_selector_for_standard_integral_impl<T, 32>
  : std::conditional<std::signed_integral<T>, claim_cv_t<T, i64>, claim_cv_t<T, u64>>
{
};

template <typename T>
struct make_higher_precision_selector_for_standard_integral_impl<T, 64>
  : std::conditional<std::signed_integral<T>, claim_cv_t<T, i128>, claim_cv_t<T, u128>>
{
};

// in case std::integral treats i/u128 as a standard integer type (e.g. -std=gnu++ mode)
template <typename T>
struct make_higher_precision_selector_for_standard_integral_impl<T, 128>
  : std::conditional<
      twos_complement_signed_integral<T>, claim_cv_t<T, integer::i<256>>, claim_cv_t<T, integer::u<256>>
    >
{
};

template <typename T>
struct make_higher_precision_selector_for_standard_integral
  : make_higher_precision_selector_for_standard_integral_impl<
      T, (precision_bits_v<T>) < (precision_bits_v<i32>) ? 0 : precision_bits_v<T>
    >
{
};

template <typename T, typename = std::remove_cv_t<T>, bool = arbitrary_precision_integral<T>>
struct make_higher_precision_selector_for_custom_integral;

template <typename T>
struct make_higher_precision_selector_for_custom_integral<T, i128, false> : claim_cv<T, integer::i<256>>
{
};

template <typename T>
struct make_higher_precision_selector_for_custom_integral<T, u128, false> : claim_cv<T, integer::u<256>>
{
};

template <typename T, usize Bits>
struct make_higher_precision_selector_for_custom_integral<T, integer::i<Bits>, false>
  : claim_cv<T, integer::i<Bits * 2>>
{
};

template <typename T, usize Bits>
struct make_higher_precision_selector_for_custom_integral<T, integer::u<Bits>, false>
  : claim_cv<T, integer::u<Bits * 2>>
{
};

template <typename T>
struct make_higher_precision_selector_for_custom_integral<T, std::remove_cv_t<T>, true>
{
  using type = T;
};

template <typename T, typename = std::remove_cv_t<T>, bool = arbitrary_precision_floating_point<T>>
struct make_higher_precision_selector_for_floating_point;

template <typename T>
struct make_higher_precision_selector_for_floating_point<T, f32, false> : claim_cv<T, f64>
{
};

template <typename T>
struct make_higher_precision_selector_for_floating_point<T, f64, false> : claim_cv<T, f80>
{
};

template <typename T>
struct make_higher_precision_selector_for_floating_point<T, f80, false> : claim_cv<T, f128>
{
};

template <typename T>
struct make_higher_precision_selector_for_floating_point<T, f128, false> : claim_cv<T, ieee754::f<256>>
{
};

template <typename T, usize Bits>
struct make_higher_precision_selector_for_floating_point<T, ieee754::f<Bits>, false> : claim_cv<T, ieee754::f<Bits * 2>>
{
};

template <typename T, usize Bits>
struct make_higher_precision_selector_for_floating_point<T, ieee754::d<Bits>, false> : claim_cv<T, ieee754::d<Bits * 2>>
{
};

template <typename T>
struct make_higher_precision_selector_for_floating_point<T, std::remove_cv_t<T>, true>
{
  using type = T;
};

template <typename>
struct make_higher_precision_selector;

template <integral T>
struct make_higher_precision_selector<T>
  : std::conditional_t<
      std::integral<T>, make_higher_precision_selector_for_standard_integral<T>,
      make_higher_precision_selector_for_custom_integral<T>
    >
{
};

template <floating_point T>
struct make_higher_precision_selector<T> : make_higher_precision_selector_for_floating_point<T>
{
};

} // namespace fmia::meta

export namespace fmia::meta {

// for a fixed-precision integer type: obtain i/u32 if its precision is smaller than 32 bits, otherwise obtain a
// fixed-precision integer type that has double precision
// for a big integer type: obtain itself
// for a floating-point type: obtain a floating-point type that has double precision
// for a big decimal type: obtain itself
//
// cv-qualifiers and signedness (only for integer types) are kept
template <typename T>
using make_higher_precision = make_higher_precision_selector<T>;

template <typename T>
using make_higher_precision_t = make_higher_precision<T>::type;

} // export namespace fmia::meta
