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

export module test.aatk.math.integer;

import std;

import aatk.math.integer;

namespace test::aatk {

using namespace ::aatk;

namespace meta {

using namespace ::aatk::meta;

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
  static_assert(std::same_as<make_signed_t<int>, int> == true);
  static_assert(std::same_as<make_signed_t<const int>, const int> == true);
  static_assert(std::same_as<make_signed_t<const volatile unsigned int>, const volatile int> == true);
  static_assert(std::same_as<make_signed_t<const u64>, const i64> == true);
  static_assert(std::same_as<make_signed_t<i128>, i128> == true);
  static_assert(std::same_as<make_signed_t<u128>, i128> == true);
  static_assert(std::same_as<make_signed_t<fixed_width_integer::u<1024>>, fixed_width_integer::i<1024>> == true);
}

consteval void does_make_unsigned_work() noexcept
{
  static_assert(std::same_as<make_unsigned_t<int>, unsigned int> == true);
  static_assert(std::same_as<make_unsigned_t<const int>, const unsigned int> == true);
  static_assert(std::same_as<make_unsigned_t<const volatile unsigned int>, const volatile unsigned int> == true);
  static_assert(std::same_as<make_unsigned_t<const u64>, const u64> == true);
  static_assert(std::same_as<make_unsigned_t<i128>, u128> == true);
  static_assert(std::same_as<make_unsigned_t<u128>, u128> == true);
  static_assert(std::same_as<make_unsigned_t<fixed_width_integer::i<1024>>, fixed_width_integer::u<1024>> == true);
}

consteval void does_make_larger_width_work() noexcept
{
  static_assert(std::same_as<make_larger_width_t<int>, i64> == true);
  static_assert(std::same_as<make_larger_width_t<const int>, const i64> == true);
  static_assert(std::same_as<make_larger_width_t<const volatile unsigned int>, const volatile u64> == true);
  static_assert(std::same_as<make_larger_width_t<const u64>, const u128> == true);
  static_assert(std::same_as<make_larger_width_t<i128>, fixed_width_integer::i<256>> == true);
  static_assert(std::same_as<make_larger_width_t<u128>, fixed_width_integer::u<256>> == true);
  static_assert(std::same_as<make_larger_width_t<fixed_width_integer::u<1024>>, fixed_width_integer::u<2048>> == true);
  static_assert(std::same_as<make_larger_width_t<fixed_width_integer::i<256>>, fixed_width_integer::i<512>> == true);
}

} // namespace meta

namespace math {

export void int128_io_with_std_iostream() noexcept
{
  i128 a, b;
  u128 c, d;

  std::cin >> a >> b >> c >> d;
  std::cout << a + b << " " << c + d << "\n";
}

export void int128_io_with_std_format() noexcept
{
  i128 a, b;
  u128 c, d;

  std::cin >> a >> b >> c >> d;
  std::println("{:05} {:05}", a + b, c + d);
}

} // namespace math

} // namespace test::aatk
