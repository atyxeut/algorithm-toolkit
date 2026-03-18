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

export module aatk.util.std_extension;

export import aatk.util.std_extension.meta;

import std;

import aatk.memory.core;

namespace aatk {

namespace detail {

template <typename, std::size_t...>
struct array_impl;

template <typename T, std::size_t Dim>
struct array_impl<T, Dim>
{
  using type = std::array<T, Dim>;
};

template <typename T, std::size_t Dim, std::size_t... Dims>
struct array_impl<T, Dim, Dims...> : array_impl<typename array_impl<T, Dims...>::type, Dim>
{
};

} // namespace detail

// aatk::array<int, 3, 5, 2> arr3d {};
// same as: std::array<std::array<std::array<int, 2>, 5>, 3> arr3d {};
export template <typename T, std::size_t... Dims>
using array = detail::array_impl<T, Dims...>::type;

} // namespace aatk

export namespace aatk::meta {

template <typename>
struct is_no_cv_std_array : std::false_type
{
};

template <typename T, std::size_t N>
struct is_no_cv_std_array<std::array<T, N>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_std_array_v = is_no_cv_std_array<T>::value;

template <typename T>
using is_std_array = is_no_cv_std_array<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_std_array_v = is_std_array<T>::value;

} // namespace aatk::meta

export namespace aatk {

// sets every element of a aatk::array to val
// aatk::array<int, 3, 5, 2, 10> arr4d;
// int val = -1;
// aatk::fill_array(arr4d, val);
template <typename Elem, std::size_t Dim, typename T>
constexpr void fill_array(std::array<Elem, Dim>& arr, const T& val)
{
  if constexpr (meta::is_no_cv_std_array_v<Elem>)
    for (auto& inner_arr : arr)
      fill_array(inner_arr, val);
  else
    arr.fill(static_cast<Elem>(val));
}

// auto arr4d = aatk::make_array<int, 5, 8, 3, 2>(val);
// combines `aatk::array<int, 5, 8, 3, 2> arr4d` and `aatk::fill_array(arr4d, val)`
template <typename Elem, std::size_t... Dims, typename T>
[[nodiscard]] constexpr auto make_array(const T& val)
{
  array<Elem, Dims...> arr;
  fill_array(arr, static_cast<Elem>(val));
  return arr;
}

} // namespace aatk

namespace aatk::meta {

// use std::allocator as a default allocator
template <list_of_types CurAllocatorList, std::size_t DimCnt>
struct adjust_allocator_type_list : concat<CurAllocatorList, std::conditional_t<(length_v<CurAllocatorList>) < DimCnt, type_list<std_allocator_tag>, empty_type_list>>
{
};

template <typename CurAllocatorList, std::size_t DimCnt>
using adjust_allocator_type_list_t = adjust_allocator_type_list<CurAllocatorList, DimCnt>::type;

template <typename Elem, nonempty_list_of_types AllocatorList, typename LastAllocator = last_t<AllocatorList>>
struct cur_dim_allocator
{
  using type = LastAllocator;
};

template <typename Elem, typename AllocatorList>
struct cur_dim_allocator<Elem, AllocatorList, std_allocator_tag>
{
  using type = std::allocator<Elem>;
};

template <typename Elem, typename AllocatorList>
struct cur_dim_allocator<Elem, AllocatorList, std_pmr_allocator_tag>
{
  using type = std::pmr::polymorphic_allocator<Elem>;
};

template <typename Elem, typename AllocatorList>
using cur_dim_allocator_t = cur_dim_allocator<Elem, AllocatorList>::type;

} // namespace aatk::meta

namespace aatk {

namespace detail {

template <typename, std::size_t DimCnt, typename>
  requires (DimCnt != 0)
class vector_impl;

template <typename T, std::size_t DimCnt, typename InnermostDimAllocator, typename... Allocators>
class vector_impl<T, DimCnt, meta::type_list<InnermostDimAllocator, Allocators...>>
{
private:
  using adjusted_allocator_type_list_ = meta::adjust_allocator_type_list_t<meta::type_list<InnermostDimAllocator, Allocators...>, DimCnt>;
  using element_type_ = vector_impl<T, DimCnt - 1, meta::init_t<adjusted_allocator_type_list_>>::type;

public:
  using type = std::vector<element_type_, meta::cur_dim_allocator_t<element_type_, adjusted_allocator_type_list_>>;
};

template <typename T, typename Allocator>
class vector_impl<T, 1, meta::type_list<Allocator>>
{
public:
  using type = std::vector<T, meta::cur_dim_allocator_t<T, meta::type_list<Allocator>>>;
};

} // namespace detail

// aatk::vector<int> vec1d;
// same as: std::vector<int> vec1d;
//
// aatk::vector<int, 4> vec4d;
// same as: std::vector<std::vector<std::vector<std::vector<int>>>> vec4d;
export template <typename T, std::size_t DimCnt = 1, typename InnermostDimAllocator = std_allocator_tag, typename... Allocators>
  requires (sizeof...(Allocators) < DimCnt)
using vector = detail::vector_impl<T, DimCnt, meta::type_list<InnermostDimAllocator, Allocators...>>::type;

namespace detail {

template <typename Elem, typename AllocatorList, typename Dim, typename... Ts>
[[nodiscard]] constexpr auto make_vector_impl(Dim first_dim_size, Ts&&... args)
{
  using adjusted_allocator_type_list = meta::adjust_allocator_type_list_t<AllocatorList, sizeof...(Ts)>;
  if constexpr (sizeof...(Ts) == 1) {
    using cur_dim_allocator_type = meta::cur_dim_allocator_t<Elem, adjusted_allocator_type_list>;
    return vector<Elem, 1, cur_dim_allocator_type>(static_cast<std::size_t>(first_dim_size), static_cast<Elem>(args)...);
  }
  else {
    using inner_allocator_type_list = meta::init_t<adjusted_allocator_type_list>;
    using inner_element_type = vector_impl<Elem, sizeof...(Ts) - 1, inner_allocator_type_list>::type;
    using cur_dim_allocator_type = meta::cur_dim_allocator_t<inner_element_type, adjusted_allocator_type_list>;
    return vector<inner_element_type, 1, cur_dim_allocator_type>(static_cast<std::size_t>(first_dim_size), make_vector_impl<Elem, inner_allocator_type_list>(std::forward<Ts>(args)...));
  }
}

} // namespace detail

// auto vec3d = aatk::make_vector<int>(x, y, z, 1);
// same as: auto vec3d = std::vector<std::vector<std::vector<int>>>(
//                         x,
//                         std::vector<std::vector<int>>(y, std::vector<int>(z, 1))
//                       );
//
// constexpr usage example (error if aatk::make_vector is not constexpr):
// std::cout << [](const std::vector<int>& vec, int sum = 0) consteval {
//   std::ranges::for_each(vec, [&sum](int elem) { return sum += elem; });
//   return sum;
// }(aatk::make_vector<int>(10, -1)) << "\n";
export template <typename Elem, typename InnermostDimAllocator = std_allocator_tag, typename... Allocators, std::integral Dim, typename... Ts>
  requires (sizeof(Dim) <= sizeof(std::size_t) && sizeof...(Ts) > 0 && sizeof...(Allocators) < sizeof...(Ts))
[[nodiscard]] constexpr auto make_vector(Dim first_dim_size, Ts&&... args)
{
  return detail::make_vector_impl<Elem, meta::type_list<InnermostDimAllocator, Allocators...>>(first_dim_size, std::forward<Ts>(args)...);
}

} // namespace aatk

export namespace aatk::meta {

template <typename T>
struct is_no_cv_std_ratio : std::false_type
{
};

template <std::intmax_t Numerator, std::intmax_t Denominator>
struct is_no_cv_std_ratio<std::ratio<Numerator, Denominator>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_std_ratio_v = is_no_cv_std_ratio<T>::value;

template <typename T>
using is_std_ratio = is_no_cv_std_ratio<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_std_ratio_v = is_std_ratio<T>::value;

template <typename T>
struct is_no_cv_std_duration : std::false_type
{
};

template <typename Rep, typename Period>
struct is_no_cv_std_duration<std::chrono::duration<Rep, Period>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_std_duration_v = is_no_cv_std_duration<T>::value;

template <typename T>
using is_std_duration = is_no_cv_std_duration<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_std_duration_v = is_std_duration<T>::value;

} // namespace aatk::meta

export namespace aatk {

template <typename T1, typename T2, std::convertible_to<std::string> Delim = std::string>
void print(std::ostream& ostr, const std::pair<T1, T2>& p, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  ostr << p.first << std::forward<Delim>(delim) << p.second;

  if (new_line)
    ostr << '\n';
}

} // namespace aatk

export template <typename T1, typename T2>
auto& operator <<(std::ostream& ostr, const std::pair<T1, T2>& p)
{
  ::aatk::print(ostr, p);
  return ostr;
}

export namespace aatk {

template <typename... Ts, std::convertible_to<std::string> Delim = std::string>
void print(std::ostream& ostr, const std::tuple<Ts...>& t, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  [&]<std::size_t... Is>(std::index_sequence<Is...>) {
    ((ostr << std::get<Is>(t) << (Is + 1 == sizeof...(Ts) ? std::string {} : delim)), ...);
  }(std::index_sequence_for<Ts...> {});

  if (new_line)
    ostr << '\n';
}

} // namespace aatk

export template <typename... Ts>
std::ostream& operator <<(std::ostream& ostr, const std::tuple<Ts...>& t)
{
  ::aatk::print(ostr, t);
  return ostr;
}

namespace aatk::meta {

namespace detail {

template <typename, typename = void>
struct is_std_ostream_interactable_impl : std::false_type
{
};

template <typename T>
struct is_std_ostream_interactable_impl<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type
{
};

} // namespace detail

// check if T has an overload of operator << of std::ostream&
// to make aatk::meta::is_std_ostream_interactable<...>::value evaluate to true, the candidate overload must be already defined above it
export template <typename T>
using is_std_ostream_interactable = detail::is_std_ostream_interactable_impl<T>;

export template <typename T>
constexpr bool is_std_ostream_interactable_v = is_std_ostream_interactable<T>::value;

} // namespace aatk::meta

export namespace aatk::meta {

template <typename T>
concept multidimentional_cstyle_array = std::rank_v<std::remove_cvref_t<T>> > 1;

} // namespace aatk::meta

namespace aatk {

// for a range whose elements can be printed by std::ostream by default
// e.g. std::vector<int>, std::vector<std::string>
export template <std::ranges::input_range Range, std::convertible_to<std::string> Delim = std::string, typename Elem = std::ranges::range_value_t<Range>>
  requires (meta::is_std_ostream_interactable_v<Elem> && !std::is_array_v<Elem>)
std::size_t print(std::ostream& ostr, Range&& range, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  for (auto it = std::ranges::begin(range), it_end = std::ranges::end(range); it != it_end; ++it)
    ostr << *it << (std::ranges::next(it) == it_end ? std::string {} : delim);

  if (new_line)
    ostr << '\n';

  return 1;
}

// for a range whose elements can not be printed by std::ostream by default
// e.g. std::vector<std::array<int, 4>>, std::vector<std::pair<int, int>>
export template <std::ranges::input_range Range, std::convertible_to<std::string> Delim = std::string, typename Elem = std::ranges::range_value_t<Range>>
  requires (!meta::is_std_ostream_interactable_v<Elem> && std::ranges::input_range<Elem>)
std::size_t print(std::ostream& ostr, Range&& range, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  std::size_t cur_dim = 0;
  for (auto it = std::ranges::begin(range), it_end = std::ranges::end(range); it != it_end; ++it) {
    cur_dim = print(ostr, *it, std::forward<Delim>(delim), false);

    const auto dimension_delim = std::string(cur_dim, '\n');
    ostr << (std::ranges::next(it) == it_end ? std::string {} : dimension_delim);
  }

  if (new_line)
    ostr << '\n';

  return cur_dim + 1;
}

export template <meta::multidimentional_cstyle_array T, std::convertible_to<std::string> Delim = std::string>
void print(std::ostream& ostr, const T& arr, Delim&& delim = std::string(1, ' '), bool new_line = false)
{
  for (auto it = std::begin(arr), it_end = std::end(arr); it != it_end; ++it) {
    print(ostr, *it, std::forward<Delim>(delim), false);

    constexpr auto dimension_delim = std::string(std::rank_v<std::remove_cvref_t<T>> - 1, '\n');
    ostr << (std::next(it) == it_end ? std::string {} : dimension_delim);
  }

  if (new_line)
    ostr << '\n';
}

} // namespace aatk

// avoid ambiguous overloads when Range is std::string&, int[2][3], ...
// some how we cannot use requires here, for clang 21.1.0, this is an infinitely recursive constraint (maybe clang's bug? gcc and msvc don't have this issue)
export template <std::ranges::input_range Range, typename = std::enable_if_t<!::aatk::meta::is_std_ostream_interactable_v<Range>>>
auto& operator <<(std::ostream& ostr, Range&& range)
{
  ::aatk::print(ostr, std::forward<Range>(range));
  return ostr;
}

// C-style arrays can decay and be output directly as a pointer, thus need a specific overload
// this overload covers multidimentional arrays
export template <typename T, std::size_t N>
auto& operator <<(std::ostream& ostr, const T (&arr)[N])
{
  ::aatk::print(ostr, arr);
  return ostr;
}
