// Copyright (C) 2026 atyxeut
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <https://www.gnu.org/licenses/>.

export module fmia.io.stl_extension;

import std;

import fmia.memory.allocator;
import fmia.meta;

export namespace fmia {

template <typename T1, typename T2, std::convertible_to<std::string> Delim = std::string>
void print(std::ostream& ostr, const std::pair<T1, T2>& p, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  ostr << p.first << std::forward<Delim>(delim) << p.second;

  if (new_line)
    ostr << '\n';
}

} // export namespace fmia

export {

template <typename T1, typename T2>
auto& operator <<(std::ostream& ostr, const std::pair<T1, T2>& p)
{
  ::fmia::print(ostr, p);
  return ostr;
}

} // export

export namespace fmia {

template <typename... Ts, std::convertible_to<std::string> Delim = std::string>
void print(std::ostream& ostr, const std::tuple<Ts...>& t, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  [&]<std::size_t... Is>(std::index_sequence<Is...>) {
    ((ostr << std::get<Is>(t) << (Is + 1 == sizeof...(Ts) ? std::string {} : delim)), ...);
  }(std::index_sequence_for<Ts...> {});

  if (new_line)
    ostr << '\n';
}

} // export namespace fmia

export {

template <typename... Ts>
auto& operator <<(std::ostream& ostr, const std::tuple<Ts...>& t)
{
  ::fmia::print(ostr, t);
  return ostr;
}

} // export

namespace fmia::meta::cpp17 {

template <typename, typename = void>
struct is_std_ostream_interactable_impl : std::false_type
{
};

template <typename T>
struct is_std_ostream_interactable_impl<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
  : std::true_type
{
};

} // namespace fmia::meta::cpp17

export namespace fmia::meta::cpp17 {

// check if T has an overload of operator << of std::ostream&
// to make fmia::meta::is_std_ostream_interactable<...>::value evaluate to true, the candidate overload must be already
// defined above it
template <typename T>
using is_std_ostream_interactable = is_std_ostream_interactable_impl<T>;

template <typename T>
constexpr bool is_std_ostream_interactable_v = is_std_ostream_interactable<T>::value;

} // export namespace fmia::meta::cpp17

export namespace fmia::meta {

// not usable: infinitely recursive constraint (llvm 22)
// however its fine for gcc and msvc
template <typename T>
concept std_ostream_interactable = requires(std::ostream& ostr, T t) { ostr << t; };

} // export namespace fmia::meta

export namespace fmia {

// clang-format off
  
// for a range whose elements can be printed by std::ostream by default
// e.g. std::vector<int>, std::vector<std::string>
template <
  std::ranges::input_range Range, std::convertible_to<std::string> Delim = std::string,
  typename Elem = std::ranges::range_value_t<Range>
> requires (meta::cpp17::is_std_ostream_interactable_v<Elem> && !std::is_array_v<Elem>)
int print(std::ostream& ostr, Range&& range, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  for (auto it = std::ranges::begin(range), it_end = std::ranges::end(range); it != it_end; ++it)
    ostr << *it << (std::ranges::next(it) == it_end ? std::string {} : delim);

  if (new_line)
    ostr << '\n';

  return 1;
}

// clang-format on

// clang-format off

// for a range whose elements can not be printed by std::ostream by default
// e.g. std::vector<std::array<int, 4>>, std::vector<std::pair<int, int>>
template <
  std::ranges::input_range Range, std::convertible_to<std::string> Delim = std::string,
  typename Elem = std::ranges::range_value_t<Range>
> requires (!meta::cpp17::is_std_ostream_interactable_v<Elem> && std::ranges::input_range<Elem>)
int print(std::ostream& ostr, Range&& range, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  int cur_dim = 0;
  for (auto it = std::ranges::begin(range), it_end = std::ranges::end(range); it != it_end; ++it) {
    cur_dim = print(ostr, *it, std::forward<Delim>(delim), false);
    const auto dimension_delim = std::string(static_cast<std::size_t>(cur_dim), '\n');
    ostr << (std::ranges::next(it) == it_end ? std::string {} : dimension_delim);
  }

  if (new_line)
    ostr << '\n';

  return cur_dim + 1;
}

// clang-format on

template <meta::multidimentional_cstyle_array T, std::convertible_to<std::string> Delim = std::string>
void print(std::ostream& ostr, const T& arr, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  for (auto it = std::begin(arr), it_end = std::end(arr); it != it_end; ++it) {
    print(ostr, *it, std::forward<Delim>(delim), false);
    const auto dimension_delim = std::string(std::rank_v<std::remove_cvref_t<T>> - 1, '\n');
    ostr << (std::next(it) == it_end ? std::string {} : dimension_delim);
  }

  if (new_line)
    ostr << '\n';
}

} // export namespace fmia

export {

// clang-format off

// avoid ambiguous overloads when Range is std::string&, int[2][3], ...
template <
  std::ranges::input_range Range,
  typename = std::enable_if_t<!::fmia::meta::cpp17::is_std_ostream_interactable_v<Range>>
> auto& operator <<(std::ostream& ostr, Range&& range)
{
  ::fmia::print(ostr, std::forward<Range>(range));
  return ostr;
}

// clang-format on

// C-style arrays can decay and be output directly as a pointer, thus need a specific overload
// this overload covers multidimentional arrays
template <typename T, std::size_t N>
  requires (!std::same_as<std::remove_cv_t<T>, char>)
auto& operator <<(std::ostream& ostr, const T (&arr)[N])
{
  ::fmia::print(ostr, arr);
  return ostr;
}

} // export
