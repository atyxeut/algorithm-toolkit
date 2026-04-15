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

import std;

import test.fmia;

int main()
{
  // test::fmia::math::int128_input_with_std_istream();
  test::fmia::math::int128_output_with_std_ostream_and_std_print();

  test::fmia::util::does_make_vector_work();

  test::fmia::util::std_pair_output_with_custom_print_and_std_ostream();
  test::fmia::util::std_tuple_output_with_custom_print_and_std_ostream();

  test::fmia::util::cstyle_string_literal_output_no_ambiguous_overloads();
  test::fmia::util::common_ranges_output_with_custom_print_and_std_ostream();
  test::fmia::util::multidimensional_ranges_output_with_custom_print_and_std_ostream();
}
