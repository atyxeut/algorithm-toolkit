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

export module test.fmia.util.std_extension.io;

import std;

import fmia.util.std_extension.container;
import fmia.util.std_extension.io;

namespace test::fmia::util {

using namespace ::fmia;

} // namespace test::fmia::util

export namespace test::fmia::util {

void std_pair_output_with_custom_print_and_std_ostream() noexcept
{
  constexpr auto p = std::make_pair("printing a pair", 3.14159);

  // print using ` | ` as the delimiter
  print(std::cout, p, " | ", true);

  // operator << uses ` ` as the delimiter
  std::cout << p << '\n';

  std::cout << '\n';
}

void std_tuple_output_with_custom_print_and_std_ostream() noexcept
{
  constexpr auto t = std::make_tuple("printing a tuple", 3.14159f, 3.1415926535, 123456, 1'000'000'000'000'000ll, 'A');

  // print using ` | ` as the delimiter
  print(std::cout, t, " | ", true);

  // operator << uses ` ` as the delimiter
  std::cout << t << '\n';

  std::cout << '\n';
}

void common_ranges_output_with_custom_print_and_std_ostream() noexcept
{
  int arr1[] {1, 2, 3, 4, 5, 6, 7};
  print(std::cout, arr1, " | ", true);
  std::cout << arr1 << '\n';

  std::cout << '\n';

  char ca[] {'1', '2', '3', '4'};
  print(std::cout, ca, " | ", true);
  std::cout << ca << '\n';

  std::cout << '\n';

  std::pair<std::string, int> arr2[] {
    {"abc", 1},
    {"def", 2},
    {"ghi", 3},
    {"jkl", 4},
    {"mno", 5}
  };
  print(std::cout, arr2, " | ", true);
  std::cout << arr2 << '\n';

  std::cout << '\n';

  std::vector<std::string> vec1 {"ABC", "BCD", "CDE", "DEF", "EFG"};
  print(std::cout, vec1, " | ", true);
  std::cout << vec1 << '\n';

  std::cout << '\n';

  std::vector<std::tuple<int, std::string, char>> vec2 {
    {1, "ABC", 'X'},
    {2, "BCD", 'Y'},
    {3, "CDE", 'Z'}
  };
  print(std::cout, vec2, " | ", true);
  std::cout << vec2 << '\n';

  std::cout << '\n';

  std::list<std::tuple<int, std::pair<int, std::string>, char>> list1 {
    {1, {2, "abc"}, 'a'},
    {2, {4, "def"}, 'd'},
    {3, {6, "ghi"}, 'g'},
    {4, {8, "jkl"}, 'j'},
  };
  print(std::cout, list1, " | ", true);
  std::cout << list1 << '\n';

  std::cout << '\n';

  std::map<std::string, char> map1 {
    {"abc", 'A'},
    {"def", 'B'},
    {"ghi", 'C'},
    {"ABC", 'a'}
  };
  print(std::cout, map1, " | ", true);
  std::cout << map1 << '\n';

  std::cout << '\n';
}

void multidimensional_ranges_output_with_custom_print_and_std_ostream() noexcept
{

  auto vec2d = make_vector<double>(3, 4, 3.14);
  print(std::cout, vec2d, " | ", true);
  std::cout << vec2d << '\n';

  std::cout << '\n';

  std::vector<std::list<int>> vl {
    {1, 2, 3,  4},
    {2, 4, 6,  8},
    {3, 6, 9, 12}
  };
  print(std::cout, vl, " | ", true);
  std::cout << vl << '\n';

  std::cout << '\n';

  auto vec3d = make_vector<double>(3, 4, 4, 1.2345);
  print(std::cout, vec3d, " | ", true);
  std::cout << vec3d << '\n';

  std::cout << '\n';

  auto arr4d = make_array<int, 2, 2, 3, 3>(-7);
  print(std::cout, arr4d, " | ", true);
  std::cout << arr4d << '\n';

  std::cout << '\n';

  int a[5][4] {
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4}
  };
  print(std::cout, a, " | ", true);
  std::cout << a << '\n';

  std::cout << '\n';

  char b[5][4] {
    {'*', '+', '-', '/'},
    {'*', '+', '-', '/'},
    {'*', '+', '-', '/'},
    {'*', '+', '-', '/'},
    {'*', '+', '-', '/'}
  };
  print(std::cout, b, " == ", true);
  std::cout << b << '\n';

  std::cout << '\n';
}

} // namespace test::fmia::util
