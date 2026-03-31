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

export module test.fmia.meta.core;

import std;

import fmia.meta.core;

namespace test::fmia::meta {

using namespace ::fmia::meta;

} // namespace test::fmia::meta

export namespace test::fmia::meta {

consteval void do_custom_integer_sequence_helpers_work() noexcept
{
  static_assert(
    std::same_as<make_reversed_integer_sequence<int, 8>, std::integer_sequence<int, 7, 6, 5, 4, 3, 2, 1, 0>>
  );
  static_assert(std::same_as<make_reversed_index_sequence<8>, std::index_sequence<7, 6, 5, 4, 3, 2, 1, 0>>);
  static_assert(std::same_as<reversed_index_sequence_for<int, int, int, int, int>, std::index_sequence<4, 3, 2, 1, 0>>);

  static_assert(std::same_as<make_integer_sequence_of_range<int, 3, 7>, std::integer_sequence<int, 3, 4, 5, 6, 7>>);
  static_assert(std::same_as<make_index_sequence_of_range<3, 10>, std::index_sequence<3, 4, 5, 6, 7, 8, 9, 10>>);
  static_assert(
    std::same_as<make_reversed_integer_sequence_of_range<int, 3, 7>, std::integer_sequence<int, 7, 6, 5, 4, 3>>
  );
  static_assert(
    std::same_as<make_reversed_index_sequence_of_range<3, 10>, std::index_sequence<10, 9, 8, 7, 6, 5, 4, 3>>
  );

  // clang-format off
  
  static_assert(
    std::same_as<
      shift_integer_sequence_t<int, 10, std::integer_sequence<int, 1, 2, 3, 4>>,
      std::integer_sequence<int, 11, 12, 13, 14>
    >
  );
  static_assert(
    std::same_as<shift_index_sequence_t<50, std::index_sequence<1, 2, 3, 4>>, std::index_sequence<51, 52, 53, 54>>
  );

  // clang-format on
}

consteval void does_is_no_duplication_integer_sequence_work() noexcept
{
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 1, 3>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 3, 4, 4, 6>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 3, 4, 5, 6>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 1, 2, 1, 2, 1>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 3, 4, 3, 3, 3>> == false);

  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 3>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 1, 3>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 3, 4, 4, 6>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 3, 4, 5, 6>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 1, 2, 1, 2, 1>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 3, 4, 3, 3, 3>> == false);
}

} // namespace test::fmia::meta
