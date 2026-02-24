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

export module test.aatk.math.floating_point;

import std;

import aatk.math.floating_point;

namespace test::aatk::meta {

consteval void does_make_higher_precision_work()
{
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<f32>, f64> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<const f32>, const f64> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<volatile f32>, volatile f64> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<const volatile f32>, const volatile f64> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<f64>, f80> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<f80>, f128> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<f128>, ::aatk::ieee754_float::f<256>> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<::aatk::ieee754_float::f<256>>, ::aatk::ieee754_float::f<512>> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<::aatk::ieee754_float::f<1024>>, ::aatk::ieee754_float::f<2048>> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<::aatk::ieee754_float::d<256>>, ::aatk::ieee754_float::d<512>> == true);
  static_assert(std::same_as<::aatk::meta::make_higher_precision_t<::aatk::ieee754_float::d<1024>>, ::aatk::ieee754_float::d<2048>> == true);
}

} // namespace test::aatk::meta
