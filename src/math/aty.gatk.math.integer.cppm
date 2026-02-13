// Copyright 2026 atyxeut
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

// https://stackoverflow.com/a/76440171
#if _MSC_VER >= 1934
#include <__msvc_int128.hpp>
#endif

export module aty.gatk.math.integer;

import std;

import aty.gatk.util.tmp;

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

export namespace aty::gatk {

namespace tmp {

template <typename T>
concept boolean = std::same_as<std::remove_cv_t<T>, bool>;

template <typename T>
concept nonbool_standard_unsigned_integral = std::unsigned_integral<T> && !boolean<T>;

template <typename T>
concept nonbool_standard_integral = nonbool_standard_unsigned_integral<T> || std::signed_integral<T>;

template <typename T>
concept for_size_integral = std::same_as<T, u32> || std::same_as<T, usize>;

} // namespace tmp

template <tmp::nonbool_standard_unsigned_integral T>
[[nodiscard]] constexpr bool is_power_of_2(T x) noexcept
{
  return x != 0 && (x & (x - 1)) == 0;
}

namespace fixed_width_integer {

template <usize WidthBits>
  requires (WidthBits >= 128 && is_power_of_2(WidthBits))
class i
{
};

template <usize WidthBits>
  requires (WidthBits >= 128 && is_power_of_2(WidthBits))
class u
{
};

} // namespace fixed_width_integer

namespace tmp {

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

} // namespace tmp

} // namespace aty::gatk

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
using i128 = ::aty::gatk::fixed_width_integer::i<128>;
using u128 = ::aty::gatk::fixed_width_integer::u<128>;
#endif

}
// clang-format on

export namespace aty::gatk::tmp {

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

} // namespace aty::gatk::tmp

namespace aty::gatk::tmp {

template <typename T, typename = std::remove_cv_t<T>>
struct make_signed_selector
{
  using type = std::make_signed_t<T>;
};

template <typename T>
struct make_signed_selector<T, i128>
{
  using type = claim_cv_t<T, i128>;
};

template <typename T>
struct make_signed_selector<T, u128>
{
  using type = claim_cv_t<T, i128>;
};

template <typename T, usize WidthBits>
struct make_signed_selector<T, fixed_width_integer::i<WidthBits>>
{
  using type = claim_cv_t<T, fixed_width_integer::i<WidthBits>>;
};

template <typename T, usize WidthBits>
struct make_signed_selector<T, fixed_width_integer::u<WidthBits>>
{
  using type = claim_cv_t<T, fixed_width_integer::i<WidthBits>>;
};

export template <typename T>
using make_signed = make_signed_selector<T>;

export template <typename T>
using make_signed_t = make_signed<T>::type;

template <typename T, typename = std::remove_cv_t<T>>
struct make_unsigned_selector
{
  using type = std::make_unsigned_t<T>;
};

template <typename T>
struct make_unsigned_selector<T, i128>
{
  using type = claim_cv_t<T, u128>;
};

template <typename T>
struct make_unsigned_selector<T, u128>
{
  using type = claim_cv_t<T, u128>;
};

template <typename T, usize WidthBits>
struct make_unsigned_selector<T, fixed_width_integer::i<WidthBits>>
{
  using type = claim_cv_t<T, fixed_width_integer::u<WidthBits>>;
};

template <typename T, usize WidthBits>
struct make_unsigned_selector<T, fixed_width_integer::u<WidthBits>>
{
  using type = claim_cv_t<T, fixed_width_integer::u<WidthBits>>;
};

export template <typename T>
using make_unsigned = make_unsigned_selector<T>;

export template <typename T>
using make_unsigned_t = make_unsigned<T>::type;

} // namespace aty::gatk::tmp

namespace aty::gatk::tmp {

template <typename T, usize = (sizeof(T) < sizeof(i32) ? 0 : sizeof(T))>
struct make_larger_width_selector_for_standard;

template <typename T>
struct make_larger_width_selector_for_standard<T, 0>
{
  using type = std::conditional_t<std::signed_integral<T>, claim_cv_t<T, i32>, claim_cv_t<T, u32>>;
};

template <typename T>
struct make_larger_width_selector_for_standard<T, sizeof(i32)>
{
  using type = std::conditional_t<std::signed_integral<T>, claim_cv_t<T, i64>, claim_cv_t<T, u64>>;
};

template <typename T>
struct make_larger_width_selector_for_standard<T, sizeof(i64)>
{
  using type = std::conditional_t<std::signed_integral<T>, claim_cv_t<T, i128>, claim_cv_t<T, u128>>;
};

template <typename T, typename U = std::remove_cv_t<T>>
struct make_larger_width_selector_for_custom;

template <typename T>
struct make_larger_width_selector_for_custom<T, i128>
{
  using type = claim_cv_t<T, fixed_width_integer::i<256>>;
};

template <typename T>
struct make_larger_width_selector_for_custom<T, u128>
{
  using type = claim_cv_t<T, fixed_width_integer::u<256>>;
};

template <typename T, usize WidthBits>
struct make_larger_width_selector_for_custom<T, fixed_width_integer::i<WidthBits>>
{
  using type = claim_cv_t<T, fixed_width_integer::i<WidthBits * 2>>;
};

template <typename T, usize WidthBits>
struct make_larger_width_selector_for_custom<T, fixed_width_integer::u<WidthBits>>
{
  using type = claim_cv_t<T, fixed_width_integer::u<WidthBits * 2>>;
};

// for the given fixed-width integer type: obtains i/u32 if its width is smaller than 32 bits, otherwise obtains a fixed-width integer type with double width
// cv-qualifiers and signedness are kept
export template <typename T>
using make_larger_width = std::conditional_t<sizeof(T) <= sizeof(i64), make_larger_width_selector_for_standard<T>, make_larger_width_selector_for_custom<T>>;

export template <typename T>
using make_larger_width_t = make_larger_width<T>::type;

} // namespace aty::gatk::tmp
