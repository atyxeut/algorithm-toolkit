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

module;

#include <cassert>

export module test.aatk.util.stl_helper;

import std;

import aatk.memory;
import aatk.meta;
import aatk.util.stl_helper;

namespace test::aatk::util {

using namespace ::aatk;

export consteval void aatk_vector_should_be_nested_std_vector() noexcept
{
  using T1 = std::vector<double>;
  using T1_ = vector<double>;
  static_assert(std::same_as<T1, T1_>);

  using T2 = std::pmr::vector<double>;
  using T2_ = vector<double, 1, memory::std_pmr_allocator_tag>;
  static_assert(std::same_as<T2, T2_>);

  using T3 = std::pmr::vector<std::pmr::vector<std::pmr::vector<std::pmr::vector<std::pmr::vector<double>>>>>;
  using T3_ = vector<double, 5, memory::std_pmr_allocator_tag, memory::std_pmr_allocator_tag, memory::std_pmr_allocator_tag, memory::std_pmr_allocator_tag, memory::std_pmr_allocator_tag>;
  static_assert(std::same_as<T3, T3_>);

  using T4 = std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>>;
  using T4_ = vector<double, 5>;
  static_assert(std::same_as<T4, T4_>);
}

export consteval void every_empty_allocator_template_parameter_of_aatk_vector_defaults_to_std_allocator() noexcept
{
  using T1 = std::pmr::vector<double>;
  using T1_ = vector<double, 1, std::pmr::polymorphic_allocator<double>>;
  static_assert(std::same_as<T1, T1_>);

  using T2 = std::vector<std::vector<std::pmr::vector<int>>>;
  using T2_ = vector<int, 3, std::pmr::polymorphic_allocator<int>>;
  using T2__ = vector<int, 3, std::pmr::polymorphic_allocator<int>, std::allocator<std::vector<int, std::pmr::polymorphic_allocator<int>>>>;
  static_assert(std::same_as<T2, T2_> && std::same_as<T2_, T2__>);
}

export void does_make_vector_work()
{
  auto get_v1 = [] consteval noexcept
  {
    return std::vector<double>(3, 3.14);
  };
  auto get_v1_ = [] consteval noexcept
  {
    return make_vector<double>(3, 3.14);
  };
  static_assert(get_v1() == get_v1_());

  // std::pmr::polymorphic_allocator cannot be used in constexpr context
  auto v2 = std::vector<std::pmr::vector<std::vector<double>>>(3, std::pmr::vector<std::vector<double>>(4, std::vector<double>(5, 3.14)));
  auto v2_ = make_vector<double, std::allocator<double>, std::pmr::polymorphic_allocator<std::vector<double>>>(3, 4, 5, 3.14);
  auto v2__ = make_vector<double, memory::std_allocator_tag, memory::std_pmr_allocator_tag>(3, 4, 5, 3.14);
  assert(v2 == v2_ && v2_ == v2__);
}

export void std_pair_output_with_custom_print_and_std_ostream() noexcept
{
  constexpr auto p = std::make_pair("printing a pair", 3.14159);

  // print using ` | ` as the delimiter
  print(std::cout, p, " | ", true);

  // operator << uses ` ` as the delimiter
  std::cout << p << '\n';

  std::cout << '\n';
}

export void std_tuple_output_with_custom_print_and_std_ostream() noexcept
{
  constexpr auto t = std::make_tuple("printing a tuple", 3.14159f, 3.1415926535, 123'456, 1'000'000'000'000'000ll, 'A');

  // print using ` | ` as the delimiter
  print(std::cout, t, " | ", true);

  // operator << uses ` ` as the delimiter
  std::cout << t << '\n';

  std::cout << '\n';
}

} // namespace test::aatk::util
