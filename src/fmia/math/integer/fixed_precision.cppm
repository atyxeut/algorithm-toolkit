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

export module fmia.math.integer.fixed_precision;

import std;

import fmia.meta.cv_qualifier;

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

} // export

export namespace fmia::meta {

template <typename T>
concept size_integral = std::same_as<T, u32> || std::same_as<T, usize>;

} // export namespace fmia::meta

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

} // export namespace fmia::meta

export namespace fmia {

template <meta::nonbool_standard_unsigned_integral T>
[[nodiscard]] constexpr bool is_power_of_2(T x) noexcept
{
  return std::popcount(x) == 1;
}

} // export namespace fmia

// forward declarations
export namespace fmia::integer {

template <usize Bits>
  requires (Bits >= 128 && is_power_of_2(Bits))
class i;

template <usize Bits>
  requires (Bits >= 128 && is_power_of_2(Bits))
class u;

} // export namespace fmia::integer

// clang-format off

export {

#if defined(__SIZEOF_INT128__) || _MSC_VER >= 1934
  #ifdef __SIZEOF_INT128__
    // https://gcc.gnu.org/onlinedocs/gcc/_005f_005fint128.html
    __extension__ using i128 = __int128;
    __extension__ using u128 = unsigned __int128;
  #else
    using i128 = std::_Signed128;
    using u128 = std::_Unsigned128;
  #endif

  auto& operator >>(std::istream& istr, u128& n)
  {
    std::string buffer;
    istr >> buffer;
  
    n = 0;
    for (char ch : buffer)
      n = n * 10 + static_cast<u128>(ch - '0');
  
    return istr;
  }
  
  auto& operator >>(std::istream& istr, i128& n)
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
  
  auto& operator <<(std::ostream& ostr, u128 n)
  {
    if (n == 0)
      return ostr << '0';
  
    std::string buffer;
    for (; n; n /= 10)
      buffer += static_cast<char>(n % 10 + '0');
    std::reverse(buffer.begin(), buffer.end());
    return ostr << buffer;
  }

  auto& operator <<(std::ostream& ostr, i128 n)
  {
    if (n == std::numeric_limits<i128>::min())
      ostr << '-' << static_cast<u128>(n);
    else if (n < 0)
      ostr << '-' << static_cast<u128>(-n);
    else
      ostr << static_cast<u128>(n);
    return ostr;
  }
#else
  using i128 = ::fmia::integer::i<128>;
  using u128 = ::fmia::integer::u<128>;
#endif

} // export

// clang-format on

export namespace fmia::meta {

template <typename>
struct is_no_cv_custom_twos_complement_signed_integral : std::false_type
{
};

template <usize Bits>
struct is_no_cv_custom_twos_complement_signed_integral<integer::i<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_twos_complement_signed_integral_v =
  is_no_cv_custom_twos_complement_signed_integral<T>::value;

template <typename T>
using is_custom_twos_complement_signed_integral = is_no_cv_custom_twos_complement_signed_integral<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_twos_complement_signed_integral_v = is_custom_twos_complement_signed_integral<T>::value;

template <typename T>
concept twos_complement_signed_integral =
  std::signed_integral<T> || std::same_as<std::remove_cv_t<T>, i128> || is_custom_twos_complement_signed_integral_v<T>;

template <typename>
struct is_no_cv_custom_unsigned_integral : std::false_type
{
};

template <usize Bits>
struct is_no_cv_custom_unsigned_integral<integer::u<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_custom_unsigned_integral_v = is_no_cv_custom_unsigned_integral<T>::value;

template <typename T>
using is_custom_unsigned_integral = is_no_cv_custom_unsigned_integral<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_custom_unsigned_integral_v = is_custom_unsigned_integral<T>::value;

template <typename T>
concept custom_twos_complement_signed_integral = is_custom_twos_complement_signed_integral_v<T>;

template <typename T>
concept unsigned_integral =
  std::unsigned_integral<T> || std::same_as<std::remove_cv_t<T>, u128> || is_custom_unsigned_integral_v<T>;

template <typename T>
concept nonbool_unsigned_integral = unsigned_integral<T> && !boolean<T>;

template <typename T>
concept fixed_precision_integral = twos_complement_signed_integral<T> || unsigned_integral<T>;

template <typename T>
concept nonbool_fixed_precision_integral = fixed_precision_integral<T> && !boolean<T>;

template <typename T>
concept nothrow_integral = fixed_precision_integral<T>;

} // export namespace fmia::meta

namespace fmia::meta {

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
struct make_signed_selector<T, integer::i<Bits>> : claim_cv<T, integer::i<Bits>>
{
};

template <typename T, usize Bits>
struct make_signed_selector<T, integer::u<Bits>> : claim_cv<T, integer::i<Bits>>
{
};

} // namespace fmia::meta

export namespace fmia::meta {

template <fixed_precision_integral T>
using make_signed = make_signed_selector<T>;

template <fixed_precision_integral T>
using make_signed_t = make_signed<T>::type;

} // export namespace fmia::meta

namespace fmia::meta {

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
struct make_unsigned_selector<T, integer::i<Bits>> : claim_cv<T, integer::u<Bits>>
{
};

template <typename T, usize Bits>
struct make_unsigned_selector<T, integer::u<Bits>> : claim_cv<T, integer::u<Bits>>
{
};

} // namespace fmia::meta

export namespace fmia::meta {

template <fixed_precision_integral T>
using make_unsigned = make_unsigned_selector<T>;

template <fixed_precision_integral T>
using make_unsigned_t = make_unsigned<T>::type;

} // export namespace fmia::meta

export namespace fmia::meta {

// unsigned integers implicitly modulo 2^(Bits)
template <typename T>
struct is_no_cv_modular_integral : std::bool_constant<std::same_as<T, std::remove_cv_t<T>> && unsigned_integral<T>>
{
};

template <typename T>
constexpr bool is_no_cv_modular_integral_v = is_no_cv_modular_integral<T>::value;

template <typename T>
using is_modular_integral = is_no_cv_modular_integral<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_modular_integral_v = is_modular_integral<T>::value;

template <typename T>
concept modular_integral = is_modular_integral_v<T>;

} // export namespace fmia::meta
