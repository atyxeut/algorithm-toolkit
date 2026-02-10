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

export module aty.gatk.math.integer;

import std;

import aty.gatk.math.core;
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
concept size_integral = std::same_as<T, u32> || std::same_as<T, usize>;

} // namespace tmp

template <tmp::nonbool_standard_unsigned_integral T>
constexpr bool is_power_of_2(T x) noexcept
{
  return (x & (x - 1)) == 0;
}

namespace fixed_integer {

template <usize Bits>
  requires (Bits >= 128 && is_power_of_2(Bits))
class i
{
};

template <usize Bits>
  requires (Bits >= 128 && is_power_of_2(Bits))
class u
{
};

} // namespace fixed_integer

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
using i128 = ::aty::gatk::fixed_integer::i<128>;
using u128 = ::aty::gatk::fixed_integer::u<128>;
#endif

}
// clang-format on

export namespace aty::gatk::tmp {

template <typename>
struct is_custom_fixed_signed_integral : std::false_type
{
};

template <usize Bits>
struct is_custom_fixed_signed_integral<fixed_integer::i<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_custom_fixed_signed_integral_v = is_custom_fixed_signed_integral<T>::value;

template <typename T>
concept fixed_signed_integral = std::signed_integral<T> || std::same_as<std::remove_cv_t<T>, i128> || is_custom_fixed_signed_integral_v<T>;

template <typename>
struct is_custom_fixed_unsigned_integral : std::false_type
{
};

template <usize Bits>
struct is_custom_fixed_unsigned_integral<fixed_integer::u<Bits>> : std::true_type
{
};

template <typename T>
constexpr bool is_custom_fixed_unsigned_integral_v = is_custom_fixed_unsigned_integral<T>::value;

template <typename T>
concept fixed_unsigned_integral = std::unsigned_integral<T> || std::same_as<std::remove_cv_t<T>, u128> || is_custom_fixed_unsigned_integral_v<T>;

template <typename T>
concept nonbool_fixed_unsigned_integral = fixed_unsigned_integral<T> && !boolean<T>;

template <typename T>
concept fixed_integral = fixed_signed_integral<T> || fixed_unsigned_integral<T>;

template <typename T>
concept nonbool_fixed_integral = fixed_integral<T> && !boolean<T>;

} // namespace aty::gatk::tmp
