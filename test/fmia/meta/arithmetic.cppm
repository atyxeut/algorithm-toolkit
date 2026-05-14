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

export module test.fmia.meta.arithmetic;

import std;

import fmia.math.integer;
import fmia.math.floating_point;
import fmia.meta.arithmetic;

using namespace fmia;
using namespace fmia::meta;

consteval void does_make_higher_precision_for_integral_work() noexcept
{
  namespace fp = twos_complement;

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
  static_assert(std::same_as<make_higher_precision_t<f128>, ieee754::f<256>> == true);
  static_assert(std::same_as<make_higher_precision_t<ieee754::f<256>>, ieee754::f<512>> == true);
  static_assert(std::same_as<make_higher_precision_t<ieee754::f<1024>>, ieee754::f<2048>> == true);
  static_assert(std::same_as<make_higher_precision_t<ieee754::d<256>>, ieee754::d<512>> == true);
  static_assert(std::same_as<make_higher_precision_t<ieee754::d<1024>>, ieee754::d<2048>> == true);
}

consteval void does_compare_precision_for_integral_work() noexcept
{
  namespace fp = twos_complement;

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
  namespace fp = ieee754;

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
