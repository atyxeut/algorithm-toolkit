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

export module aatk.util.stl_helper;

import std;

import aatk.memory;
import aatk.meta;

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
struct array_impl<T, Dim, Dims...>
{
  using type = std::array<typename array_impl<T, Dims...>::type, Dim>;
};

} // namespace detail

// aatk::array<int, 3, 5, 2> arr3d {};
// same as: std::array<std::array<std::array<int, 2>, 5>, 3> arr3d {};
export template <typename T, std::size_t... Dims>
using array = detail::array_impl<T, Dims...>::type;

export namespace meta {

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

} // namespace meta

// sets every element of a aatk::array to val
// aatk::array<int, 3, 5, 2, 10> arr4d;
// int val = -1;
// aatk::fill_array(arr4d, val);
export template <typename Elem, std::size_t Dim, typename T>
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
export template <typename Elem, std::size_t... Dims, typename T>
[[nodiscard]] constexpr auto make_array(const T& val)
{
  array<Elem, Dims...> arr;
  fill(arr, static_cast<Elem>(val));
  return arr;
}

// use std::allocator as a default allocator
template <meta::list_of_types CurAllocatorList, std::size_t DimCnt>
struct adjust_allocator_type_list : meta::concat<CurAllocatorList, std::conditional_t<(meta::length_v<CurAllocatorList>) < DimCnt, meta::type_list<memory::std_allocator_tag>, meta::empty_type_list>>
{
};

template <typename CurAllocatorList, std::size_t DimCnt>
using adjust_allocator_type_list_t = adjust_allocator_type_list<CurAllocatorList, DimCnt>::type;

template <typename Elem, meta::nonempty_list_of_types AllocatorList, typename LastAllocator = meta::last_t<AllocatorList>>
struct cur_dim_allocator
{
  using type = LastAllocator;
};

template <typename Elem, typename AllocatorList>
struct cur_dim_allocator<Elem, AllocatorList, memory::std_allocator_tag>
{
  using type = std::allocator<Elem>;
};

template <typename Elem, typename AllocatorList>
struct cur_dim_allocator<Elem, AllocatorList, memory::std_pmr_allocator_tag>
{
  using type = std::pmr::polymorphic_allocator<Elem>;
};

template <typename Elem, typename AllocatorList>
using cur_dim_allocator_t = cur_dim_allocator<Elem, AllocatorList>::type;

namespace detail {

template <typename, std::size_t DimCnt, typename>
  requires (DimCnt != 0)
class vector_impl;

template <typename T, std::size_t DimCnt, typename InnermostDimAllocator, typename... Allocators>
class vector_impl<T, DimCnt, meta::type_list<InnermostDimAllocator, Allocators...>>
{
  using adjusted_allocator_type_list_ = adjust_allocator_type_list_t<meta::type_list<InnermostDimAllocator, Allocators...>, DimCnt>;
  using element_type_ = vector_impl<T, DimCnt - 1, meta::init_t<adjusted_allocator_type_list_>>::type;

public:
  using type = std::vector<element_type_, cur_dim_allocator_t<element_type_, adjusted_allocator_type_list_>>;
};

template <typename T, typename Allocator>
class vector_impl<T, 1, meta::type_list<Allocator>>
{
public:
  using type = std::vector<T, cur_dim_allocator_t<T, meta::type_list<Allocator>>>;
};

} // namespace detail

// aatk::vector<int> vec1d;
// same as: std::vector<int> vec1d;
//
// aatk::vector<int, 4> vec4d;
// same as: std::vector<std::vector<std::vector<std::vector<int>>>> vec4d;
export template <typename T, std::size_t DimCnt = 1, typename InnermostDimAllocator = memory::std_allocator_tag, typename... Allocators>
  requires (sizeof...(Allocators) < DimCnt)
using vector = detail::vector_impl<T, DimCnt, meta::type_list<InnermostDimAllocator, Allocators...>>::type;

namespace detail {

template <typename Elem, typename AllocatorList, typename Dim, typename... Ts>
[[nodiscard]] constexpr auto make_vector_impl(Dim first_dim_size, Ts&&... args)
{
  using adjusted_allocator_type_list = adjust_allocator_type_list_t<AllocatorList, sizeof...(Ts)>;
  if constexpr (sizeof...(Ts) == 1) {
    using cur_dim_allocator_type = cur_dim_allocator_t<Elem, adjusted_allocator_type_list>;
    return vector<Elem, 1, cur_dim_allocator_type>(static_cast<std::size_t>(first_dim_size), static_cast<Elem>(args)...);
  }
  else {
    using inner_allocator_type_list = meta::init_t<adjusted_allocator_type_list>;
    using inner_element_type = vector_impl<Elem, sizeof...(Ts) - 1, inner_allocator_type_list>::type;
    using cur_dim_allocator_type = cur_dim_allocator_t<inner_element_type, adjusted_allocator_type_list>;
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
export template <typename Elem, typename InnermostDimAllocator = memory::std_allocator_tag, typename... Allocators, std::integral Dim, typename... Ts>
  requires (sizeof(Dim) <= sizeof(std::size_t) && sizeof...(Ts) > 0 && sizeof...(Allocators) < sizeof...(Ts))
[[nodiscard]] constexpr auto make_vector(Dim first_dim_size, Ts&&... args)
{
  return detail::make_vector_impl<Elem, meta::type_list<InnermostDimAllocator, Allocators...>>(first_dim_size, std::forward<Ts>(args)...);
}

export namespace meta {

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

} // namespace meta

} // namespace aatk
