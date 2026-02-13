// Copyright 2026 atyxeut
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

export module aty.gatk.util.stl_helper;

import std;

namespace aty::gatk {

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

// aty::gatk::array<int, 3, 5, 2> arr3d {};
//   same as: std::array<std::array<std::array<int, 2>, 5>, 3> arr3d {};
export template <typename T, std::size_t... Dims>
using array = array_impl<T, Dims...>::type;

export namespace tmp {

template <typename>
struct is_std_array : std::false_type
{
};

template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type
{
};

template <typename T>
constexpr bool is_std_array_v = is_std_array<T>::value;

} // namespace tmp

// aty::gatk::array<int, 3, 5, 2, 10> arr4d;
// int val = -1;
// aty::gatk::fill_array(arr4d, val);
//   set every element of a aty::gatk::array to val
export template <typename TElem, std::size_t Dim, typename T>
constexpr void fill_array(std::array<TElem, Dim>& arr, const T& val)
{
  if constexpr (tmp::is_std_array_v<TElem>)
    for (auto& inner_arr : arr)
      fill_array(inner_arr, val);
  else
    arr.fill(static_cast<TElem>(val));
}

// auto arr4d = aty::gatk::make_array<int, 5, 8, 3, 2>(val);
//   combines aty::gatk::array<int, 5, 8, 3, 2> arr4d;
//        and aty::gatk::fill_array(arr4d, val);
export template <typename TElem, std::size_t... Dims, typename T>
[[nodiscard]] constexpr auto make_array(const T& val)
{
  array<TElem, Dims...> arr;
  fill(arr, static_cast<TElem>(val));
  return arr;
}

template <typename T, std::size_t DimCnt>
  requires (DimCnt > 0)
struct vector_impl
{
  using type = std::vector<typename vector_impl<T, DimCnt - 1>::type>;
};

template <typename T>
struct vector_impl<T, 1>
{
  using type = std::vector<T>;
};

// aty::gatk::vector<int> vec1d;
//   same as: std::vector<int> vec1d;
// aty::gatk::vector<int, 4> vec4d;
//   same as: std::vector<std::vector<std::vector<std::vector<int>>>> vec4d;
export template <typename T, std::size_t DimCnt = 1>
using vector = vector_impl<T, DimCnt>::type;

// auto vec3d = aty::gatk::make_vector<int>(x, y, z, 1);
//   same as: auto vec3d = std::vector<std::vector<std::vector<int>>>(
//                           x,
//                           std::vector<std::vector<int>>(y, std::vector<int>(z, 1))
//                         );
// constexpr usage example (error if aty::gatk::make_vector is not constexpr):
//   std::cout << [](const std::vector<int>& vec, int sum = 0) consteval
//   {
//     std::ranges::for_each(vec, [&sum](int elem) { return sum += elem; });
//     return sum;
//   }(aty::gatk::make_vector<int>(10, -1)) << "\n";
export template <typename TElem, std::integral TDim, typename... Ts>
  requires (sizeof(TDim) <= sizeof(std::size_t) && sizeof...(Ts) > 0)
[[nodiscard]] constexpr auto make_vector(TDim first_dim_size, Ts&&... args)
{
  if constexpr (sizeof...(Ts) == 1)
    return std::vector(static_cast<std::size_t>(first_dim_size), static_cast<TElem>(args)...);
  else
    return std::vector(static_cast<std::size_t>(first_dim_size), make_vector<TElem>(std::forward<Ts>(args)...));
}

} // namespace aty::gatk
