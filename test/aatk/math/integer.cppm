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

namespace test::aatk::meta {

consteval void does_is_boolean_work() noexcept
{
  static_assert(::aatk::meta::is_boolean_v<int> == false);
  static_assert(::aatk::meta::is_boolean_v<bool> == true);
  static_assert(::aatk::meta::is_boolean_v<const bool> == true);
  static_assert(::aatk::meta::is_boolean_v<volatile bool> == true);
  static_assert(::aatk::meta::is_boolean_v<const volatile bool> == true);

  static_assert(::aatk::meta::boolean<int> == false);
  static_assert(::aatk::meta::boolean<bool> == true);
  static_assert(::aatk::meta::boolean<const bool> == true);
  static_assert(::aatk::meta::boolean<volatile bool> == true);
  static_assert(::aatk::meta::boolean<const volatile bool> == true);
}

consteval void does_make_signed_work() noexcept
{
  static_assert(std::same_as<::aatk::meta::make_signed_t<int>, int> == true);
  static_assert(std::same_as<::aatk::meta::make_signed_t<const int>, const int> == true);
  static_assert(std::same_as<::aatk::meta::make_signed_t<const volatile unsigned int>, const volatile int> == true);
  static_assert(std::same_as<::aatk::meta::make_signed_t<const u64>, const i64> == true);
  static_assert(std::same_as<::aatk::meta::make_signed_t<i128>, i128> == true);
  static_assert(std::same_as<::aatk::meta::make_signed_t<u128>, i128> == true);
  static_assert(std::same_as<::aatk::meta::make_signed_t<::aatk::fixed_width_integer::u<1024>>, ::aatk::fixed_width_integer::i<1024>> == true);
}

consteval void does_make_unsigned_work() noexcept
{
  static_assert(std::same_as<::aatk::meta::make_unsigned_t<int>, unsigned int> == true);
  static_assert(std::same_as<::aatk::meta::make_unsigned_t<const int>, const unsigned int> == true);
  static_assert(std::same_as<::aatk::meta::make_unsigned_t<const volatile unsigned int>, const volatile unsigned int> == true);
  static_assert(std::same_as<::aatk::meta::make_unsigned_t<const u64>, const u64> == true);
  static_assert(std::same_as<::aatk::meta::make_unsigned_t<i128>, u128> == true);
  static_assert(std::same_as<::aatk::meta::make_unsigned_t<u128>, u128> == true);
  static_assert(std::same_as<::aatk::meta::make_unsigned_t<::aatk::fixed_width_integer::i<1024>>, ::aatk::fixed_width_integer::u<1024>> == true);
}

consteval void does_make_larger_width_work() noexcept
{
  static_assert(std::same_as<::aatk::meta::make_larger_width_t<int>, i64> == true);
  static_assert(std::same_as<::aatk::meta::make_larger_width_t<const int>, const i64> == true);
  static_assert(std::same_as<::aatk::meta::make_larger_width_t<const volatile unsigned int>, const volatile u64> == true);
  static_assert(std::same_as<::aatk::meta::make_larger_width_t<const u64>, const u128> == true);
  static_assert(std::same_as<::aatk::meta::make_larger_width_t<i128>, ::aatk::fixed_width_integer::i<256>> == true);
  static_assert(std::same_as<::aatk::meta::make_larger_width_t<u128>, ::aatk::fixed_width_integer::u<256>> == true);
  static_assert(std::same_as<::aatk::meta::make_larger_width_t<::aatk::fixed_width_integer::u<1024>>, ::aatk::fixed_width_integer::u<2048>> == true);
  static_assert(std::same_as<::aatk::meta::make_larger_width_t<::aatk::fixed_width_integer::i<256>>, ::aatk::fixed_width_integer::i<512>> == true);
}

} // namespace test::aatk::meta
