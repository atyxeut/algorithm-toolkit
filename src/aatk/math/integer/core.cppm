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

// included for libc++ `_LIBCPP_VERSION` macro
#include <version>

export module aatk.math.integer.core;

import std;

import aatk.meta;

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

export namespace aatk {

namespace meta {

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
concept for_size_integral = std::same_as<T, u32> || std::same_as<T, usize>;

} // namespace meta

template <meta::nonbool_standard_unsigned_integral T>
[[nodiscard]] constexpr bool is_power_of_2(T x) noexcept
{
  return x != 0 && (x & (x - 1)) == 0;
}

// forward declaration
namespace fixed_width_integer {

template <usize WidthBits>
  requires (WidthBits >= 128 && is_power_of_2(WidthBits))
class i;

template <usize WidthBits>
  requires (WidthBits >= 128 && is_power_of_2(WidthBits))
class u;

} // namespace fixed_width_integer

namespace meta {

template <typename>
struct is_no_cv_custom_fixed_width_signed_integral : std::false_type
{
};

template <usize WidthBits>
struct is_no_cv_custom_fixed_width_signed_integral<fixed_width_integer::i<WidthBits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_fixed_width_signed_integral_v = is_no_cv_custom_fixed_width_signed_integral<T>::value;

template <typename T>
using is_custom_fixed_width_signed_integral = is_no_cv_custom_fixed_width_signed_integral<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_fixed_width_signed_integral_v = is_custom_fixed_width_signed_integral<T>::value;

template <typename>
struct is_no_cv_custom_fixed_width_unsigned_integral : std::false_type
{
};

template <usize WidthBits>
struct is_no_cv_custom_fixed_width_unsigned_integral<fixed_width_integer::u<WidthBits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_fixed_width_unsigned_integral_v = is_no_cv_custom_fixed_width_unsigned_integral<T>::value;

template <typename T>
using is_custom_fixed_width_unsigned_integral = is_no_cv_custom_fixed_width_unsigned_integral<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_fixed_width_unsigned_integral_v = is_custom_fixed_width_unsigned_integral<T>::value;

} // namespace meta

} // namespace aatk

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
using i128 = ::aatk::fixed_width_integer::i<128>;
using u128 = ::aatk::fixed_width_integer::u<128>;
#endif

}
// clang-format on

export inline auto& operator >>(std::istream& istr, u128& n)
{
  std::string buffer;
  istr >> buffer;

  n = 0;
  for (auto ch : buffer)
    n = n * 10 + static_cast<u128>(ch - '0');

  return istr;
}

export inline auto& operator >>(std::istream& istr, i128& n)
{
  std::string buffer;
  istr >> buffer;

  u128 mag = 0;
  const int sgn = buffer[0] == '-' ? -1 : 1;
  for (std::size_t i = sgn < 0; i < buffer.size(); ++i)
    mag = mag * 10 + static_cast<u128>(buffer[i] - '0');
  if (mag > std::numeric_limits<i128>::max() || sgn > 0)
    n = static_cast<i128>(mag);
  else
    n = -static_cast<i128>(mag);

  return istr;
}

export inline auto& operator <<(std::ostream& ostr, u128 n)
{
  if (n == 0)
    return ostr << 0;

  std::string buffer;
  for (; n; n /= 10)
    buffer += static_cast<char>(n % 10 + '0');
  std::ranges::reverse(buffer);

  return ostr << buffer;
}

export inline auto& operator <<(std::ostream& ostr, i128 n)
{
  if (n == std::numeric_limits<i128>::min())
    return ostr << '-' << static_cast<u128>(n);

  if (n < 0)
    return ostr << '-' << static_cast<u128>(-n);

  return ostr << static_cast<u128>(n);
}

// template <>
// struct std::formatter<u128>
// {
//   constexpr auto parse(auto& parse_ctx) { return parse_ctx.begin(); }

//   auto format(u128 n, auto& fmt_ctx) const
//   {
//     std::string buffer;
//     if (n == 0) {
//       buffer += '0';
//     }
//     else {
//       for (; n; n /= 10)
//         buffer += static_cast<char>(n % 10 + '0');
//       std::ranges::reverse(buffer);
//     }

//     return std::format_to(fmt_ctx.out(), "{}", buffer);
//   }
// };

// template <>
// struct std::formatter<i128>
// {
//   constexpr auto parse(auto& parse_ctx) { return parse_ctx.begin(); }

//   auto format(i128 n, auto& fmt_ctx) const
//   {
//     if (n == std::numeric_limits<i128>::min())
//       return std::format_to(fmt_ctx.out(), "-{}", static_cast<u128>(n));

//     if (n < 0)
//       return std::format_to(fmt_ctx.out(), "-{}", static_cast<u128>(-n));

//     return std::format_to(fmt_ctx.out(), "{}", static_cast<u128>(n));
//   }
// };

export namespace aatk::meta {

template <typename T>
concept fixed_width_signed_integral = std::signed_integral<T> || std::same_as<std::remove_cv_t<T>, i128> || is_custom_fixed_width_signed_integral_v<T>;

template <typename T>
concept fixed_width_unsigned_integral = std::unsigned_integral<T> || std::same_as<std::remove_cv_t<T>, u128> || is_custom_fixed_width_unsigned_integral_v<T>;

template <typename T>
concept nonbool_fixed_width_unsigned_integral = fixed_width_unsigned_integral<T> && !boolean<T>;

template <typename T>
concept fixed_width_integral = fixed_width_signed_integral<T> || fixed_width_unsigned_integral<T>;

template <typename T>
concept nonbool_fixed_width_integral = fixed_width_integral<T> && !boolean<T>;

} // namespace aatk::meta

namespace aatk::meta {

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

template <typename T, usize WidthBits>
struct make_signed_selector<T, fixed_width_integer::i<WidthBits>> : claim_cv<T, fixed_width_integer::i<WidthBits>>
{
};

template <typename T, usize WidthBits>
struct make_signed_selector<T, fixed_width_integer::u<WidthBits>> : claim_cv<T, fixed_width_integer::i<WidthBits>>
{
};

} // namespace detail

export template <fixed_width_integral T>
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

template <typename T, usize WidthBits>
struct make_unsigned_selector<T, fixed_width_integer::i<WidthBits>> : claim_cv<T, fixed_width_integer::u<WidthBits>>
{
};

template <typename T, usize WidthBits>
struct make_unsigned_selector<T, fixed_width_integer::u<WidthBits>> : claim_cv<T, fixed_width_integer::u<WidthBits>>
{
};

} // namespace detail

export template <fixed_width_integral T>
using make_unsigned = detail::make_unsigned_selector<T>;

export template <typename T>
using make_unsigned_t = make_unsigned<T>::type;

} // namespace aatk::meta

export namespace aatk::meta {

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
concept signed_integral = fixed_width_signed_integral<T> || is_big_integer_v<T>;

template <typename T>
concept integral = signed_integral<T> || fixed_width_unsigned_integral<T>;

template <typename T>
concept nonbool_integral = integral<T> && !boolean<T>;

} // namespace aatk::meta

namespace aatk::meta {

namespace detail {

template <typename T, usize TypeSize>
struct make_larger_width_selector_for_standard_impl;

template <typename T>
struct make_larger_width_selector_for_standard_impl<T, 0> : std::conditional<std::signed_integral<T>, claim_cv_t<T, i32>, claim_cv_t<T, u32>>
{
};

template <typename T>
struct make_larger_width_selector_for_standard_impl<T, sizeof(i32)> : std::conditional<std::signed_integral<T>, claim_cv_t<T, i64>, claim_cv_t<T, u64>>
{
};

template <typename T>
struct make_larger_width_selector_for_standard_impl<T, sizeof(i64)> : std::conditional<std::signed_integral<T>, claim_cv_t<T, i128>, claim_cv_t<T, u128>>
{
};

// in case std::integral treats i/u128 as standard integer type (e.g. -std=gnu++ mode)
template <typename T>
struct make_larger_width_selector_for_standard_impl<T, sizeof(i128)> : std::conditional<signed_integral<T>, claim_cv_t<T, fixed_width_integer::i<256>>, claim_cv_t<T, fixed_width_integer::u<256>>>
{
};

template <typename T>
struct make_larger_width_selector_for_standard : make_larger_width_selector_for_standard_impl<T, sizeof(T) < sizeof(i32) ? 0 : sizeof(T)>
{
};

template <typename T, typename = std::remove_cv_t<T>, bool = is_big_integer_v<T>>
struct make_larger_width_selector_for_custom;

template <typename T>
struct make_larger_width_selector_for_custom<T, i128, false> : claim_cv<T, fixed_width_integer::i<256>>
{
};

template <typename T>
struct make_larger_width_selector_for_custom<T, u128, false> : claim_cv<T, fixed_width_integer::u<256>>
{
};

template <typename T, usize WidthBits>
struct make_larger_width_selector_for_custom<T, fixed_width_integer::i<WidthBits>, false> : claim_cv<T, fixed_width_integer::i<WidthBits * 2>>
{
};

template <typename T, usize WidthBits>
struct make_larger_width_selector_for_custom<T, fixed_width_integer::u<WidthBits>, false> : claim_cv<T, fixed_width_integer::u<WidthBits * 2>>
{
};

template <typename T>
struct make_larger_width_selector_for_custom<T, std::remove_cv_t<T>, true>
{
  using type = T;
};

} // namespace detail

// for the given fixed-width integer type: obtain i/u32 if its width is smaller than 32 bits, otherwise obtain a fixed-width integer type with double width
// if the given type is a big integer type: obtain itself
// cv-qualifiers and signedness are kept
export template <integral T>
using make_larger_width = std::conditional_t<std::integral<T>, detail::make_larger_width_selector_for_standard<T>, detail::make_larger_width_selector_for_custom<T>>;

export template <typename T>
using make_larger_width_t = make_larger_width<T>::type;

} // namespace aatk::meta
