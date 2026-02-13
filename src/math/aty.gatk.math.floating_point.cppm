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

export module aty.gatk.math.floating_point;

import std;

import aty.gatk.math.integer;
import aty.gatk.util.tmp;

// clang-format off
export {

using f32 = float;       // precision: 6 to 9 decimal places
using f64 = double;      // precision: 15 to 17 decimal places
using f80 = long double; // precision: 18 to 20 decimal places (to gcc)

}
// clang-format on

export namespace aty::gatk {

namespace ieee754_float {

// IEEE 754 binary floating-point
template <usize PrecisionBits>
  requires (PrecisionBits >= 128 && is_power_of_2(PrecisionBits))
class f
{
};

// IEEE 754 decimal floating-point
template <usize PrecisionBits>
  requires (PrecisionBits >= 32 && is_power_of_2(PrecisionBits))
class d
{
};

} // namespace ieee754_float

namespace tmp {

template <typename>
struct is_custom_ieee754_binary_floating_point : std::false_type
{
};

template <usize PrecisionBits>
struct is_custom_ieee754_binary_floating_point<ieee754_float::f<PrecisionBits>> : std::true_type
{
};

template <typename T>
constexpr bool is_custom_ieee754_binary_floating_point_v = is_custom_ieee754_binary_floating_point<T>::value;

template <typename>
struct is_custom_ieee754_decimal_floating_point : std::false_type
{
};

template <usize PrecisionBits>
struct is_custom_ieee754_decimal_floating_point<ieee754_float::d<PrecisionBits>> : std::true_type
{
};

template <typename T>
constexpr bool is_custom_ieee754_decimal_floating_point_v = is_custom_ieee754_decimal_floating_point<T>::value;

} // namespace tmp

} // namespace aty::gatk

// f128 precision: 33 to 35 decimal places
#ifdef __SIZEOF_FLOAT128__
// https://gcc.gnu.org/onlinedocs/gcc/Floating-Types.html
export __extension__ using f128 = __float128;
#else
export using f128 = ::aty::gatk::ieee754_float::f<128>;
#endif

export namespace aty::gatk::tmp {

template <typename T>
concept ieee754_binary_floating_point = std::floating_point<T> || std::same_as<std::remove_cv_t<T>, f128> || is_custom_ieee754_binary_floating_point_v<T>;

template <typename T>
concept ieee754_decimal_floating_point = is_custom_ieee754_decimal_floating_point_v<T>;

template <typename T>
concept ieee754_floating_point = ieee754_binary_floating_point<T> || ieee754_decimal_floating_point<T>;

} // namespace aty::gatk::tmp

namespace aty::gatk::tmp {

template <typename T, typename = std::remove_cv_t<T>>
struct make_higher_precision_selector;

template <typename T>
struct make_higher_precision_selector<T, f32>
{
  using type = claim_cv_t<T, f64>;
};

template <typename T>
struct make_higher_precision_selector<T, f64>
{
  using type = claim_cv_t<T, f80>;
};

template <typename T>
struct make_higher_precision_selector<T, f80>
{
  using type = claim_cv_t<T, f128>;
};

template <typename T>
struct make_higher_precision_selector<T, f128>
{
  using type = claim_cv_t<T, ieee754_float::f<256>>;
};

template <typename T, usize PrecisionBits>
struct make_higher_precision_selector<T, ieee754_float::f<PrecisionBits>>
{
  using type = claim_cv_t<T, ieee754_float::f<PrecisionBits * 2>>;
};

template <typename T, usize PrecisionBits>
struct make_higher_precision_selector<T, ieee754_float::d<PrecisionBits>>
{
  using type = claim_cv_t<T, ieee754_float::d<PrecisionBits * 2>>;
};

// for the given floating-point type, obtains a floating-point type that has double precision
// cv-qualifiers are kept
export template <typename T>
using make_higher_precision = make_higher_precision_selector<T>;

export template <typename T>
using make_higher_precision_t = make_higher_precision<T>::type;

} // namespace aty::gatk::tmp
