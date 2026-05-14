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

export module fmia.meta.range;

import std;

export namespace fmia::meta {

template <typename T, typename U>
concept range_of = std::ranges::range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept input_range_of = std::ranges::input_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept forward_range_of = std::ranges::forward_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept bidirectional_range_of = std::ranges::bidirectional_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept random_access_range_of = std::ranges::random_access_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept contiguous_range_of = std::ranges::contiguous_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;


} // export namespace fmia::meta
