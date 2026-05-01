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

export module fmia.meta.cv_qualifier;

import std;

export namespace fmia::meta {

template <typename T>
struct has_cv : std::bool_constant<std::is_const_v<T> || std::is_volatile_v<T>>
{
};

template <typename T>
constexpr bool has_cv_v = has_cv<T>::value;

template <typename T>
struct is_cv : std::bool_constant<std::is_const_v<T> && std::is_volatile_v<T>>
{
};

template <typename T>
constexpr bool is_cv_v = is_cv<T>::value;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename From, typename To, bool = std::is_const_v<From>, bool = std::is_volatile_v<From>>
struct claim_cv_selector;

// branch 1: has both cv qualifiers
template <typename From, typename To>
struct claim_cv_selector<From, To, true, true> : std::add_cv<To>
{
};

// branch 2: has only const qualifier
template <typename From, typename To>
struct claim_cv_selector<From, To, true, false> : std::add_const<To>
{
};

// branch 3: has only volatile qualifier
template <typename From, typename To>
struct claim_cv_selector<From, To, false, true> : std::add_volatile<To>
{
};

// branch 4: has no cv qualifiers
template <typename From, typename To>
struct claim_cv_selector<From, To, false, false>
{
  using type = To;
};

} // namespace fmia::meta

export namespace fmia::meta {

// extract the cv-qualifiers of From and apply them to To
template <typename From, typename To>
using claim_cv = claim_cv_selector<From, To>;

template <typename From, typename To>
using claim_cv_t = claim_cv<From, To>::type;

} // export namespace fmia::meta
