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

export module fmia.memory.core;

import std;

import fmia.math.core;
import fmia.meta;

export namespace fmia {

// represent std::allocator<T>
struct std_allocator_tag
{
};

// represent std::pmr::polymorphic_allocator<T>
struct std_pmr_allocator_tag
{
};

} // namespace fmia

export namespace fmia::meta {

// allocator type order: dim_n, dim_(n - 1), ..., dim_1
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

// used in recursion, add a std::allocator as the default allocator, if the length of the allocator type list
// < dim count
template <list_of_types CurAllocatorList, std::size_t DimCnt>
struct adjust_allocator_type_list
  : concat<
      CurAllocatorList,
      std::conditional_t<(length_v<CurAllocatorList>) < DimCnt, type_list<std_allocator_tag>, empty_type_list>
    >
{
};

template <typename CurAllocatorList, std::size_t DimCnt>
using adjust_allocator_type_list_t = adjust_allocator_type_list<CurAllocatorList, DimCnt>::type;

} // namespace fmia::meta

export namespace fmia {

template <meta::size_integral T>
constexpr T dynamic_capacity = static_cast<T>(-1);

enum class resource_location { inplace, heap };
enum class exception_safety { basic, strong };

} // namespace fmia
