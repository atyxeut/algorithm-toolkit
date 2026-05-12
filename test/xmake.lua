-- Copyright (C) 2026 atyxeut
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <https://www.gnu.org/licenses/>.

target("test-fmia")
  set_kind("object")
  add_deps("fmia");
  add_files("fmia/**.cppm", { public = true })
target_end()

target("test-math-int128_input")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/int128_input.cpp")
target_end()

target("test-math-int128_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/int128_output.cpp")
target_end()

target("test-util-mdrange-make_vector")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/util/mdrange/make_vector.cpp")
target_end()

target("test-io-stl_extension-ranges_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/io/stl_extension/ranges_output.cpp")
target_end()

target("test-io-stl_extension-std_tuple_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/io/stl_extension/std_tuple_output.cpp")
target_end()
