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

export module fmia.util.std_extension;

export import fmia.util.std_extension.container;
export import fmia.util.std_extension.io;

import std;

export namespace fmia::meta {

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

} // namespace fmia::meta
