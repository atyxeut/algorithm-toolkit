// Copyright (C) 2026 atyxeut
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

export module test.fmia.math.core;

import std;

import fmia.math.core;

using namespace fmia;
using namespace fmia::meta;

consteval void does_is_boolean_work() noexcept
{
  static_assert(is_boolean_v<int> == false);
  static_assert(is_boolean_v<bool> == true);
  static_assert(is_boolean_v<const bool> == true);
  static_assert(is_boolean_v<volatile bool> == true);
  static_assert(is_boolean_v<const volatile bool> == true);

  static_assert(boolean<int> == false);
  static_assert(boolean<bool> == true);
  static_assert(boolean<const bool> == true);
  static_assert(boolean<volatile bool> == true);
  static_assert(boolean<const volatile bool> == true);
}

consteval void does_make_signed_work() noexcept
{
  namespace fp = fixed_precision_integer;

  static_assert(std::same_as<make_signed_t<int>, int> == true);
  static_assert(std::same_as<make_signed_t<const int>, const int> == true);
  static_assert(std::same_as<make_signed_t<const volatile unsigned int>, const volatile int> == true);
  static_assert(std::same_as<make_signed_t<const u64>, const i64> == true);
  static_assert(std::same_as<make_signed_t<i128>, i128> == true);
  static_assert(std::same_as<make_signed_t<u128>, i128> == true);
  static_assert(std::same_as<make_signed_t<fp::u<1024>>, fp::i<1024>> == true);
}

consteval void does_make_unsigned_work() noexcept
{
  namespace fp = fixed_precision_integer;

  static_assert(std::same_as<make_unsigned_t<int>, unsigned int> == true);
  static_assert(std::same_as<make_unsigned_t<const int>, const unsigned int> == true);
  static_assert(std::same_as<make_unsigned_t<const volatile unsigned int>, const volatile unsigned int> == true);
  static_assert(std::same_as<make_unsigned_t<const u64>, const u64> == true);
  static_assert(std::same_as<make_unsigned_t<i128>, u128> == true);
  static_assert(std::same_as<make_unsigned_t<u128>, u128> == true);
  static_assert(std::same_as<make_unsigned_t<fp::i<1024>>, fp::u<1024>> == true);
}

consteval void does_make_higher_precision_for_integral_work() noexcept
{
  namespace fp = fixed_precision_integer;

  static_assert(std::same_as<make_higher_precision_t<int>, i64> == true);
  static_assert(std::same_as<make_higher_precision_t<const int>, const i64> == true);
  static_assert(std::same_as<make_higher_precision_t<const volatile unsigned int>, const volatile u64> == true);
  static_assert(std::same_as<make_higher_precision_t<const u64>, const u128> == true);
  static_assert(std::same_as<make_higher_precision_t<i128>, fp::i<256>> == true);
  static_assert(std::same_as<make_higher_precision_t<u128>, fp::u<256>> == true);
  static_assert(std::same_as<make_higher_precision_t<fp::u<1024>>, fp::u<2048>> == true);
  static_assert(std::same_as<make_higher_precision_t<fp::i<256>>, fp::i<512>> == true);
}

consteval void does_make_higher_precision_for_floating_point_work() noexcept
{
  static_assert(std::same_as<make_higher_precision_t<f32>, f64> == true);
  static_assert(std::same_as<make_higher_precision_t<const f32>, const f64> == true);
  static_assert(std::same_as<make_higher_precision_t<volatile f32>, volatile f64> == true);
  static_assert(std::same_as<make_higher_precision_t<const volatile f32>, const volatile f64> == true);
  static_assert(std::same_as<make_higher_precision_t<f64>, f80> == true);
  static_assert(std::same_as<make_higher_precision_t<f80>, f128> == true);
  static_assert(std::same_as<make_higher_precision_t<f128>, ieee754_float::f<256>> == true);
  static_assert(std::same_as<make_higher_precision_t<ieee754_float::f<256>>, ieee754_float::f<512>> == true);
  static_assert(std::same_as<make_higher_precision_t<ieee754_float::f<1024>>, ieee754_float::f<2048>> == true);
  static_assert(std::same_as<make_higher_precision_t<ieee754_float::d<256>>, ieee754_float::d<512>> == true);
  static_assert(std::same_as<make_higher_precision_t<ieee754_float::d<1024>>, ieee754_float::d<2048>> == true);
}

consteval void does_compare_precision_for_integral_work() noexcept
{
  namespace fp = fixed_precision_integer;

  static_assert(compare_precision_v<int, int> == 0);
  static_assert(compare_precision_v<int, long long> == -1);
  static_assert(compare_precision_v<int, signed char> == 1);
  static_assert(!precision_comparable<int, unsigned int>);
  static_assert(!precision_comparable<unsigned int, int>);

  static_assert(compare_precision_v<int, fp::i<256>> == -1);
  static_assert(compare_precision_v<fp::i<256>, int> == 1);
  static_assert(!precision_comparable<int, fp::u<256>>);
  static_assert(!precision_comparable<fp::u<256>, int>);

  static_assert(compare_precision_v<int, i128> == -1);
  static_assert(compare_precision_v<i128, int> == 1);
  static_assert(!precision_comparable<int, u128>);
  static_assert(!precision_comparable<u128, int>);

  static_assert(compare_precision_v<i128, i128> == 0);
  static_assert(!precision_comparable<i128, u128>);

  static_assert(compare_precision_v<i128, fp::i<128>> == 0);
  static_assert(compare_precision_v<fp::u<128>, u128> == 0);

  static_assert(compare_precision_v<fp::i<128>, fp::i<128>> == 0);
  static_assert(compare_precision_v<fp::i<128>, fp::i<1024>> == -1);
  static_assert(compare_precision_v<fp::i<256>, fp::i<128>> == 1);

  static_assert(compare_precision_v<fp::u<128>, fp::u<128>> == 0);
  static_assert(compare_precision_v<fp::u<128>, fp::u<1024>> == -1);
  static_assert(compare_precision_v<fp::u<256>, fp::u<128>> == 1);

  static_assert(!precision_comparable<fp::i<128>, fp::u<128>>);
  static_assert(!precision_comparable<fp::u<128>, fp::i<128>>);
}

consteval void does_compare_precision_for_floating_point_work() noexcept
{
  namespace fp = ieee754_float;

  static_assert(compare_precision_v<float, float> == 0);
  static_assert(compare_precision_v<float, double> == -1);
  static_assert(compare_precision_v<double, float> == 1);

  static_assert(compare_precision_v<double, f128> == -1);
  static_assert(compare_precision_v<f128, f128> == 0);

  static_assert(compare_precision_v<double, fp::f<128>> == -1);

  static_assert(compare_precision_v<fp::f<128>, fp::f<128>> == 0);
  static_assert(compare_precision_v<fp::f<128>, fp::f<256>> == -1);
  static_assert(compare_precision_v<fp::f<256>, fp::f<128>> == 1);

  static_assert(compare_precision_v<fp::d<128>, fp::d<128>> == 0);
  static_assert(compare_precision_v<fp::d<128>, fp::d<256>> == -1);
  static_assert(compare_precision_v<fp::d<256>, fp::d<128>> == 1);

  static_assert(!precision_comparable<fp::f<128>, fp::d<128>>);
  static_assert(!precision_comparable<fp::f<128>, fp::d<256>>);
  static_assert(!precision_comparable<fp::d<128>, fp::f<256>>);
}

export {

void int128_input_with_std_istream() noexcept
{
  i128 a, b;
  u128 c, d;

  std::cin >> a >> b >> c >> d;
  std::println("i128: {}\ni128: {}\nu128: {}\nu128: {}\n", a, b, c, d);
}

// implies test for std::format
void int128_output_with_std_ostream_and_std_print() noexcept
{
  i128 a = -30, b = -40;
  u128 c = 30, d = 40;

  std::cout << a + b << ' ' << c + d << '\n';
  std::println("{:05} {:05}\n", a + b, c + d);
}

} // export
