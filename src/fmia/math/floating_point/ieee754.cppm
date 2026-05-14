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

export module fmia.math.floating_point.ieee754;

import std;

import fmia.math.integer.twos_complement;

// clang-format off

export {

using f32 = float;       // precision: 6 to 9 decimal places
using f64 = double;      // precision: 15 to 17 decimal places
using f80 = long double; // precision: 18 to 20 decimal places (note that on MSVC long double is double)

} // export

// clang-format on

// forward declarations
export namespace fmia::ieee754 {

// IEEE 754 binary floating-point
template <usize Bits>
  requires (Bits >= 128 && is_power_of_2(Bits))
class f;

// IEEE 754 decimal floating-point
template <usize Bits>
  requires (Bits >= 32 && is_power_of_2(Bits))
class d;

} // export namespace fmia::ieee754

// clang-format off

export {

// f128 precision: 33 to 35 decimal places
#ifdef __SIZEOF_FLOAT128__
  // https://gcc.gnu.org/onlinedocs/gcc/Floating-Types.html
  __extension__ using f128 = __float128;
#else
  using f128 = ::fmia::ieee754::f<128>;
#endif

} // export

// clang-format on

export namespace fmia::meta {

template <typename>
struct is_no_cv_custom_ieee754_binary_floating_point : std::false_type
{
};

template <usize Bits>
struct is_no_cv_custom_ieee754_binary_floating_point<ieee754::f<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_ieee754_binary_floating_point_v =
  is_no_cv_custom_ieee754_binary_floating_point<T>::value;

template <typename T>
using is_custom_ieee754_binary_floating_point = is_no_cv_custom_ieee754_binary_floating_point<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_ieee754_binary_floating_point_v = is_custom_ieee754_binary_floating_point<T>::value;

template <typename T>
concept ieee754_binary_floating_point =
  std::floating_point<T> || std::same_as<std::remove_cv_t<T>, f128> || is_custom_ieee754_binary_floating_point_v<T>;

template <typename>
struct is_no_cv_custom_ieee754_decimal_floating_point : std::false_type
{
};

template <usize Bits>
struct is_no_cv_custom_ieee754_decimal_floating_point<ieee754::d<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_ieee754_decimal_floating_point_v =
  is_no_cv_custom_ieee754_decimal_floating_point<T>::value;

template <typename T>
using is_custom_ieee754_decimal_floating_point = is_no_cv_custom_ieee754_decimal_floating_point<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_ieee754_decimal_floating_point_v = is_custom_ieee754_decimal_floating_point<T>::value;

template <typename T>
concept ieee754_decimal_floating_point = is_custom_ieee754_decimal_floating_point_v<T>;

template <typename T>
concept ieee754_floating_point = ieee754_binary_floating_point<T> || ieee754_decimal_floating_point<T>;

template <typename T>
concept nothrow_floating_point = ieee754_floating_point<T>;

} // export namespace fmia::meta
