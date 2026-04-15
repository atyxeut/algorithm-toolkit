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

// https://stackoverflow.com/a/76440171
#if _MSC_VER >= 1934
#include <__msvc_int128.hpp>
#endif

export module fmia.math.core;

// basic arithmetic types and metaprogramming utils

import std;

import fmia.meta;

// clang-format off

export {

using i8  = std::int8_t;
using u8  = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

using isize = std::ptrdiff_t;
using usize = std::size_t;

}

// clang-format on

export namespace fmia::meta {

template <typename T>
struct is_no_cv_boolean : std::bool_constant<std::same_as<T, bool>>
{
};

template <typename T>
constexpr bool is_no_cv_boolean_v = is_no_cv_boolean<T>::value;

template <typename T>
using is_boolean = is_no_cv_boolean<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_boolean_v = is_boolean<T>::value;

template <typename T>
concept boolean = is_boolean_v<std::remove_cv_t<T>>;

template <typename T>
concept nonbool_standard_unsigned_integral = std::unsigned_integral<T> && !boolean<T>;

template <typename T>
concept nonbool_standard_integral = std::integral<T> && !boolean<T>;

template <typename T>
concept size_integral = std::same_as<T, u32> || std::same_as<T, usize>;

} // namespace fmia::meta

export namespace fmia {

template <meta::nonbool_standard_unsigned_integral T>
[[nodiscard]] constexpr bool is_power_of_2(T x) noexcept
{
  return x != 0 & (x & x - 1) == 0;
}

} // namespace fmia

// forward declaration
export namespace fmia::fixed_precision_integer {

template <usize Bits>
  requires (Bits >= 128 && is_power_of_2(Bits))
class i;

template <usize Bits>
  requires (Bits >= 128 && is_power_of_2(Bits))
class u;

} // namespace fmia::fixed_precision_integer

export namespace fmia::meta {

template <typename>
struct is_no_cv_custom_fixed_precision_signed_integral : std::false_type
{
};

template <usize Bits>
struct is_no_cv_custom_fixed_precision_signed_integral<fixed_precision_integer::i<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_fixed_precision_signed_integral_v =
  is_no_cv_custom_fixed_precision_signed_integral<T>::value;

template <typename T>
using is_custom_fixed_precision_signed_integral = is_no_cv_custom_fixed_precision_signed_integral<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_fixed_precision_signed_integral_v = is_custom_fixed_precision_signed_integral<T>::value;

template <typename>
struct is_no_cv_custom_fixed_precision_unsigned_integral : std::false_type
{
};

template <usize Bits>
struct is_no_cv_custom_fixed_precision_unsigned_integral<fixed_precision_integer::u<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_fixed_precision_unsigned_integral_v =
  is_no_cv_custom_fixed_precision_unsigned_integral<T>::value;

template <typename T>
using is_custom_fixed_precision_unsigned_integral =
  is_no_cv_custom_fixed_precision_unsigned_integral<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_fixed_precision_unsigned_integral_v = is_custom_fixed_precision_unsigned_integral<T>::value;

} // namespace fmia::meta

// clang-format off

export {

#ifdef __SIZEOF_INT128__
// https://gcc.gnu.org/onlinedocs/gcc/_005f_005fint128.html
__extension__ using i128 = __int128;
__extension__ using u128 = unsigned __int128;
#elif _MSC_VER >= 1934
using i128 = std::_Signed128;
using u128 = std::_Unsigned128;
#else
using i128 = ::fmia::fixed_precision_integer::i<128>;
using u128 = ::fmia::fixed_precision_integer::u<128>;
#endif

}

// clang-format on

export auto& operator >>(std::istream& istr, u128& n)
{
  std::string buffer;
  istr >> buffer;

  n = 0;
  for (char ch : buffer)
    n = n * 10 + static_cast<u128>(ch - '0');

  return istr;
}

export auto& operator >>(std::istream& istr, i128& n)
{
  std::string buffer;
  istr >> buffer;

  const int sgn = buffer[0] == '-' ? -1 : 1;

  u128 mag = 0;
  for (usize i = sgn < 0; i < buffer.size(); ++i)
    mag = mag * 10 + static_cast<u128>(buffer[i] - '0');

  if (mag > std::numeric_limits<i128>::max() || sgn > 0)
    n = static_cast<i128>(mag);
  else
    n = -static_cast<i128>(mag);

  return istr;
}

export auto& operator <<(std::ostream& ostr, u128 n)
{
  if (n == 0) {
    ostr << '0';
    return ostr;
  }

  std::string buffer;
  for (; n; n /= 10)
    buffer += static_cast<char>(n % 10 + '0');
  std::reverse(buffer.begin(), buffer.end());
  ostr << buffer;
  return ostr;
}

export auto& operator <<(std::ostream& ostr, i128 n)
{
  if (n == std::numeric_limits<i128>::min())
    ostr << '-' << static_cast<u128>(n);
  else if (n < 0)
    ostr << '-' << static_cast<u128>(-n);
  else
    ostr << static_cast<u128>(n);

  return ostr;
}

export namespace fmia::meta {

template <typename T>
concept fixed_precision_signed_integral =
  std::signed_integral<T> || std::same_as<std::remove_cv_t<T>, i128> || is_custom_fixed_precision_signed_integral_v<T>;

template <typename T>
concept fixed_precision_unsigned_integral = std::unsigned_integral<T> || std::same_as<std::remove_cv_t<T>, u128>
                                            || is_custom_fixed_precision_unsigned_integral_v<T>;

template <typename T>
concept nonbool_fixed_precision_unsigned_integral = fixed_precision_unsigned_integral<T> && !boolean<T>;

template <typename T>
concept fixed_precision_integral = fixed_precision_signed_integral<T> || fixed_precision_unsigned_integral<T>;

template <typename T>
concept nonbool_fixed_precision_integral = fixed_precision_integral<T> && !boolean<T>;

} // namespace fmia::meta

namespace fmia::meta {

namespace detail {

template <typename T, typename = std::remove_cv_t<T>>
struct make_signed_selector : std::make_signed<T>
{
};

template <typename T>
struct make_signed_selector<T, i128> : claim_cv<T, i128>
{
};

template <typename T>
struct make_signed_selector<T, u128> : claim_cv<T, i128>
{
};

template <typename T, usize Bits>
struct make_signed_selector<T, fixed_precision_integer::i<Bits>> : claim_cv<T, fixed_precision_integer::i<Bits>>
{
};

template <typename T, usize Bits>
struct make_signed_selector<T, fixed_precision_integer::u<Bits>> : claim_cv<T, fixed_precision_integer::i<Bits>>
{
};

} // namespace detail

export template <fixed_precision_integral T>
using make_signed = detail::make_signed_selector<T>;

export template <typename T>
using make_signed_t = make_signed<T>::type;

namespace detail {

template <typename T, typename = std::remove_cv_t<T>>
struct make_unsigned_selector : std::make_unsigned<T>
{
};

template <typename T>
struct make_unsigned_selector<T, i128> : claim_cv<T, u128>
{
};

template <typename T>
struct make_unsigned_selector<T, u128> : claim_cv<T, u128>
{
};

template <typename T, usize Bits>
struct make_unsigned_selector<T, fixed_precision_integer::i<Bits>> : claim_cv<T, fixed_precision_integer::u<Bits>>
{
};

template <typename T, usize Bits>
struct make_unsigned_selector<T, fixed_precision_integer::u<Bits>> : claim_cv<T, fixed_precision_integer::u<Bits>>
{
};

} // namespace detail

export template <fixed_precision_integral T>
using make_unsigned = detail::make_unsigned_selector<T>;

export template <typename T>
using make_unsigned_t = make_unsigned<T>::type;

} // namespace fmia::meta

export namespace fmia::meta {

template <typename>
struct is_no_cv_big_integer : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_big_integer_v = is_no_cv_big_integer<T>::value;

template <typename T>
using is_big_integer = is_no_cv_big_integer<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_big_integer_v = is_big_integer<T>::value;

template <typename T>
concept arbitrary_precision_integral = is_big_integer_v<T>;

template <typename T>
concept signed_integral = fixed_precision_signed_integral<T> || arbitrary_precision_integral<T>;

template <typename T>
concept integral = fixed_precision_integral<T> || arbitrary_precision_integral<T>;

template <typename T>
concept nonbool_integral = integral<T> && !boolean<T>;

} // namespace fmia::meta

namespace fmia::meta::detail {

template <typename T, usize TypeSize>
struct make_higher_precision_selector_for_standard_integral_impl;

template <typename T>
struct make_higher_precision_selector_for_standard_integral_impl<T, 0>
  : std::conditional<std::signed_integral<T>, claim_cv_t<T, i32>, claim_cv_t<T, u32>>
{
};

template <typename T>
struct make_higher_precision_selector_for_standard_integral_impl<T, sizeof(i32)>
  : std::conditional<std::signed_integral<T>, claim_cv_t<T, i64>, claim_cv_t<T, u64>>
{
};

template <typename T>
struct make_higher_precision_selector_for_standard_integral_impl<T, sizeof(i64)>
  : std::conditional<std::signed_integral<T>, claim_cv_t<T, i128>, claim_cv_t<T, u128>>
{
};

// in case std::integral treats i/u128 as standard integer type (e.g. -std=gnu++ mode)
template <typename T>
struct make_higher_precision_selector_for_standard_integral_impl<T, sizeof(i128)>
  : std::conditional<
      signed_integral<T>, claim_cv_t<T, fixed_precision_integer::i<256>>, claim_cv_t<T, fixed_precision_integer::u<256>>
    >
{
};

template <typename T>
struct make_higher_precision_selector_for_standard_integral
  : make_higher_precision_selector_for_standard_integral_impl<T, sizeof(T) < sizeof(i32) ? 0 : sizeof(T)>
{
};

template <typename T, typename = std::remove_cv_t<T>, bool = is_big_integer_v<T>>
struct make_higher_precision_selector_for_custom_integral;

template <typename T>
struct make_higher_precision_selector_for_custom_integral<T, i128, false> : claim_cv<T, fixed_precision_integer::i<256>>
{
};

template <typename T>
struct make_higher_precision_selector_for_custom_integral<T, u128, false> : claim_cv<T, fixed_precision_integer::u<256>>
{
};

template <typename T, usize Bits>
struct make_higher_precision_selector_for_custom_integral<T, fixed_precision_integer::i<Bits>, false>
  : claim_cv<T, fixed_precision_integer::i<Bits * 2>>
{
};

template <typename T, usize Bits>
struct make_higher_precision_selector_for_custom_integral<T, fixed_precision_integer::u<Bits>, false>
  : claim_cv<T, fixed_precision_integer::u<Bits * 2>>
{
};

template <typename T>
struct make_higher_precision_selector_for_custom_integral<T, std::remove_cv_t<T>, true>
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

} // namespace fmia::meta::detail

// clang-format off

export {

using f32 = float;       // precision: 6 to 9 decimal places
using f64 = double;      // precision: 15 to 17 decimal places
using f80 = long double; // precision: 18 to 20 decimal places (probably, on MSVC long double is double)

}

// clang-format on

// forward declaration
export namespace fmia::ieee754_float {

// IEEE 754 binary floating-point
template <usize Bits>
  requires (Bits >= 128 && is_power_of_2(Bits))
class f;

// IEEE 754 decimal floating-point
template <usize Bits>
  requires (Bits >= 32 && is_power_of_2(Bits))
class d;

} // namespace fmia::ieee754_float

export namespace fmia::meta {

template <typename>
struct is_no_cv_custom_ieee754_binary_floating_point : std::false_type
{
};

template <usize Bits>
struct is_no_cv_custom_ieee754_binary_floating_point<ieee754_float::f<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_ieee754_binary_floating_point_v =
  is_no_cv_custom_ieee754_binary_floating_point<T>::value;

template <typename T>
using is_custom_ieee754_binary_floating_point = is_no_cv_custom_ieee754_binary_floating_point<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_ieee754_binary_floating_point_v = is_custom_ieee754_binary_floating_point<T>::value;

template <typename>
struct is_no_cv_custom_ieee754_decimal_floating_point : std::false_type
{
};

template <usize Bits>
struct is_no_cv_custom_ieee754_decimal_floating_point<ieee754_float::d<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_ieee754_decimal_floating_point_v =
  is_no_cv_custom_ieee754_decimal_floating_point<T>::value;

template <typename T>
using is_custom_ieee754_decimal_floating_point = is_no_cv_custom_ieee754_decimal_floating_point<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_ieee754_decimal_floating_point_v = is_custom_ieee754_decimal_floating_point<T>::value;

} // namespace fmia::meta

// clang-format off

export {

// f128 precision: 33 to 35 decimal places
#ifdef __SIZEOF_FLOAT128__
// https://gcc.gnu.org/onlinedocs/gcc/Floating-Types.html
__extension__ using f128 = __float128;
#else
using f128 = ::fmia::ieee754_float::f<128>;
#endif

}

// clang-format on

export namespace fmia::meta {

template <typename T>
concept ieee754_binary_floating_point =
  std::floating_point<T> || std::same_as<std::remove_cv_t<T>, f128> || is_custom_ieee754_binary_floating_point_v<T>;

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
concept arbitrary_precision_floating_point = is_big_decimal_v<T>;

template <typename T>
concept floating_point = ieee754_floating_point<T> || arbitrary_precision_floating_point<T>;

} // namespace fmia::meta

namespace fmia::meta::detail {

template <typename T, typename = std::remove_cv_t<T>, bool = is_big_decimal_v<T>>
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
struct make_higher_precision_selector_for_floating_point<T, f128, false> : claim_cv<T, ieee754_float::f<256>>
{
};

template <typename T, usize Bits>
struct make_higher_precision_selector_for_floating_point<T, ieee754_float::f<Bits>, false>
  : claim_cv<T, ieee754_float::f<Bits * 2>>
{
};

template <typename T, usize Bits>
struct make_higher_precision_selector_for_floating_point<T, ieee754_float::d<Bits>, false>
  : claim_cv<T, ieee754_float::d<Bits * 2>>
{
};

template <typename T>
struct make_higher_precision_selector_for_floating_point<T, std::remove_cv_t<T>, true>
{
  using type = T;
};

template <floating_point T>
struct make_higher_precision_selector<T> : make_higher_precision_selector_for_floating_point<T>
{
};

} // namespace fmia::meta::detail

export namespace fmia::meta {

template <typename T>
concept arithmetic = integral<T> || floating_point<T>;

// for a fixed-precision integer type: obtain i/u32 if its precision is smaller than 32 bits, otherwise obtain a
// fixed-precision integer type that has double precision
// for a big integer type: obtain itself
// for a floating-point type: obtain a floating-point type that has double precision
// for a big decimal type: obtain itself
//
// cv-qualifiers and signedness (only for integer types) are kept
template <typename T>
using make_higher_precision = detail::make_higher_precision_selector<T>;

template <typename T>
using make_higher_precision_t = make_higher_precision<T>::type;

} // namespace fmia::meta

namespace fmia::big_integer::naive {

}

namespace fmia::big_integer::naive::delayed_carry {

}

namespace fmia::big_integer::naive::immediate_carry {

}
