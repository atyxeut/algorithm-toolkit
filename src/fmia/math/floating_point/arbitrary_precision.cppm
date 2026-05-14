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

export module fmia.math.floating_point.arbitrary_precision;

import std;

import fmia.math.floating_point.ieee754;

export namespace fmia::meta {

template <typename>
struct is_no_cv_arbitrary_precision_floating_point : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_arbitrary_precision_floating_point_v = is_no_cv_arbitrary_precision_floating_point<T>::value;

template <typename T>
using is_arbitrary_precision_floating_point = is_no_cv_arbitrary_precision_floating_point<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_arbitrary_precision_floating_point_v = is_arbitrary_precision_floating_point<T>::value;

template <typename T>
concept arbitrary_precision_floating_point = is_arbitrary_precision_floating_point_v<T>;

template <typename T>
concept floating_point = ieee754_floating_point<T> || arbitrary_precision_floating_point<T>;

} // export namespace fmia::meta
