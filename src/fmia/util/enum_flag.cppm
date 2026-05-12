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

export module fmia.util.enum_flag;

import std;

export namespace fmia::meta {

template <typename>
struct is_enum_flag : std::false_type
{
};

template <typename T>
constexpr bool is_enum_flag_v = is_enum_flag<T>::value;

template <typename T>
concept enum_flag = is_enum_flag_v<T>;

} // export namespace fmia::meta

export {

template <::fmia::meta::enum_flag T>
[[nodiscard]] constexpr auto operator &(T a, T b) noexcept
{
  return static_cast<T>(std::to_underlying(a) & std::to_underlying(b));
};

template <::fmia::meta::enum_flag T>
constexpr auto& operator &=(T& a, T b) noexcept
{
  return a = a & b;
};

template <::fmia::meta::enum_flag T>
[[nodiscard]] constexpr auto operator ^(T a, T b) noexcept
{
  return static_cast<T>(std::to_underlying(a) ^ std::to_underlying(b));
};

template <::fmia::meta::enum_flag T>
constexpr auto& operator ^=(T& a, T b) noexcept
{
  return a = a ^ b;
};

template <::fmia::meta::enum_flag T>
[[nodiscard]] constexpr auto operator |(T a, T b) noexcept
{
  return static_cast<T>(std::to_underlying(a) | std::to_underlying(b));
};

template <::fmia::meta::enum_flag T>
constexpr auto& operator |=(T& a, T b) noexcept
{
  return a = a | b;
};

} // export
