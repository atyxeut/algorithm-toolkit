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

export module fmia.meta.not_same;

import std;

export namespace fmia::meta {

template <typename T, typename U>
struct not_same : std::negation<std::is_same<T, U>>
{
};

template <typename T, typename U>
constexpr bool not_same_v = not_same<T, U>::value;

template <typename T, typename U>
concept not_same_as = !std::same_as<T, U>;

template <typename T, typename U>
concept no_cvref_same_as = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

template <typename T, typename U>
concept no_cvref_not_same_as = !no_cvref_same_as<T, U>;

} // export namespace fmia::meta
