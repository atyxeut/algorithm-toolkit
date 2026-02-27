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

export module aatk.math.floating_point.core;

import std;

import aatk.math.integer.core;
import aatk.meta;

// clang-format off
export {

using f32 = float;       // precision: 6 to 9 decimal places
using f64 = double;      // precision: 15 to 17 decimal places
using f80 = long double; // precision: 18 to 20 decimal places (probably, on MSVC long double is double)

}
// clang-format on

export namespace aatk {

// forward declaration
namespace ieee754_float {

// IEEE 754 binary floating-point
template <usize PrecisionBits>
  requires (PrecisionBits >= 128 && is_power_of_2(PrecisionBits))
class f;

// IEEE 754 decimal floating-point
template <usize PrecisionBits>
  requires (PrecisionBits >= 32 && is_power_of_2(PrecisionBits))
class d;

} // namespace ieee754_float

namespace meta {

template <typename>
struct is_no_cv_custom_ieee754_binary_floating_point : std::false_type
{
};

template <usize PrecisionBits>
struct is_no_cv_custom_ieee754_binary_floating_point<ieee754_float::f<PrecisionBits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_ieee754_binary_floating_point_v = is_no_cv_custom_ieee754_binary_floating_point<T>::value;

template <typename T>
using is_custom_ieee754_binary_floating_point = is_no_cv_custom_ieee754_binary_floating_point<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_ieee754_binary_floating_point_v = is_custom_ieee754_binary_floating_point<T>::value;

template <typename>
struct is_no_cv_custom_ieee754_decimal_floating_point : std::false_type
{
};

template <usize PrecisionBits>
struct is_no_cv_custom_ieee754_decimal_floating_point<ieee754_float::d<PrecisionBits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_ieee754_decimal_floating_point_v = is_no_cv_custom_ieee754_decimal_floating_point<T>::value;

template <typename T>
using is_custom_ieee754_decimal_floating_point = is_no_cv_custom_ieee754_decimal_floating_point<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_ieee754_decimal_floating_point_v = is_custom_ieee754_decimal_floating_point<T>::value;

} // namespace meta

} // namespace aatk

// clang-format off
export {

// f128 precision: 33 to 35 decimal places
#ifdef __SIZEOF_FLOAT128__
// https://gcc.gnu.org/onlinedocs/gcc/Floating-Types.html
__extension__ using f128 = __float128;
#else
using f128 = ::aatk::ieee754_float::f<128>;
#endif

}
// clang-format on

export namespace aatk::meta {

template <typename T>
concept ieee754_binary_floating_point = std::floating_point<T> || std::same_as<std::remove_cv_t<T>, f128> || is_custom_ieee754_binary_floating_point_v<T>;

template <typename T>
concept ieee754_decimal_floating_point = is_custom_ieee754_decimal_floating_point_v<T>;

template <typename T>
concept ieee754_floating_point = ieee754_binary_floating_point<T> || ieee754_decimal_floating_point<T>;

template <typename>
struct is_no_cv_big_decimal : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_big_decimal_v = is_no_cv_big_decimal<T>::value;

template <typename T>
using is_big_decimal = is_no_cv_big_decimal<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_big_decimal_v = is_big_decimal<T>::value;

template <typename T>
concept floating_point = ieee754_floating_point<T> || is_big_decimal_v<T>;

} // namespace aatk::meta

namespace aatk::meta {

namespace detail {

template <typename T, typename = std::remove_cv_t<T>, bool = is_big_decimal_v<T>>
struct make_higher_precision_selector;

template <typename T>
struct make_higher_precision_selector<T, f32, false> : claim_cv<T, f64>
{
};

template <typename T>
struct make_higher_precision_selector<T, f64, false> : claim_cv<T, f80>
{
};

template <typename T>
struct make_higher_precision_selector<T, f80, false> : claim_cv<T, f128>
{
};

template <typename T>
struct make_higher_precision_selector<T, f128, false> : claim_cv<T, ieee754_float::f<256>>
{
};

template <typename T, usize PrecisionBits>
struct make_higher_precision_selector<T, ieee754_float::f<PrecisionBits>, false> : claim_cv<T, ieee754_float::f<PrecisionBits * 2>>
{
};

template <typename T, usize PrecisionBits>
struct make_higher_precision_selector<T, ieee754_float::d<PrecisionBits>, false> : claim_cv<T, ieee754_float::d<PrecisionBits * 2>>
{
};

template <typename T>
struct make_higher_precision_selector<T, std::remove_cv_t<T>, true>
{
  using type = T;
};

} // namespace detail

// for the given floating-point type: obtain a floating-point type that has double precision
// if the given type is a big decimal type: obtain itself
// cv-qualifiers are kept
export template <floating_point T>
using make_higher_precision = detail::make_higher_precision_selector<T>;

export template <typename T>
using make_higher_precision_t = make_higher_precision<T>::type;

} // namespace aatk::meta
