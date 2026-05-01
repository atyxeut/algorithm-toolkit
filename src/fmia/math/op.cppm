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

export module fmia.math.op;

// operator utils

import std;

import fmia.math.core;
import fmia.meta;

export namespace fmia {

enum class element_for_operator { add = 0, mul = 1 };

template <element_for_operator Operator>
struct neutral_element_tag
{
  // allow an object of this tag to be implicitly converted to int, for creating neutral elements of arithmetic types
  constexpr operator int() noexcept { return static_cast<int>(Operator); }
};

// the default behavior for is_neutral_element, other classes must overload this function if they want this utility,
//   this is an optimization for large types for which constructing and copying is costly, e.g. identity matrix
template <element_for_operator Operation, typename T>
  requires (meta::arithmetic<T>)
[[nodiscard]] constexpr bool is_neutral_element(const T& elem)
{
  if constexpr (Operation == element_for_operator::add)
    return elem == 0;
  else if constexpr (Operation == element_for_operator::mul)
    return elem == 1;
}

} // export namespace fmia

export namespace fmia {

enum class operator_property { none = 0, associative = 1, commutative = 1 << 1 };

} // export namespace fmia

export namespace fmia::meta {

template <>
struct is_enum_flag<operator_property> : std::true_type
{
};

} // export namespace fmia::meta

export {

[[nodiscard]] constexpr auto operator &(::fmia::operator_property a, ::fmia::operator_property b) noexcept
{
  return static_cast<::fmia::operator_property>(std::to_underlying(a) & std::to_underlying(b));
};

constexpr auto& operator &=(::fmia::operator_property& a, ::fmia::operator_property b) noexcept
{
  return a = a & b;
};

[[nodiscard]] constexpr auto operator ^(::fmia::operator_property a, ::fmia::operator_property b) noexcept
{
  return static_cast<::fmia::operator_property>(std::to_underlying(a) ^ std::to_underlying(b));
};

constexpr auto& operator ^=(::fmia::operator_property& a, ::fmia::operator_property b) noexcept
{
  return a = a ^ b;
};

[[nodiscard]] constexpr auto operator |(::fmia::operator_property a, ::fmia::operator_property b) noexcept
{
  return static_cast<::fmia::operator_property>(std::to_underlying(a) | std::to_underlying(b));
};

constexpr auto& operator |=(::fmia::operator_property& a, ::fmia::operator_property b) noexcept
{
  return a = a | b;
};

} // export

export namespace fmia::meta {

template <typename T>
concept multipliable = requires(T a, T b) {
  { a * b } -> std::same_as<T>;
  { a *= b } -> std::same_as<T&>;
  { T(neutral_element_tag<element_for_operator::mul> {}) } -> std::same_as<T>;
};

} // export namespace fmia::meta
