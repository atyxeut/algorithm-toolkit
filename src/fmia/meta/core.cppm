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

export module fmia.meta.core;

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

export namespace fmia::meta {

template <std::size_t N>
using index_constant = std::integral_constant<std::size_t, N>;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename>
struct make_reversed_integer_sequence_impl;

template <typename Int, Int... Is>
struct make_reversed_integer_sequence_impl<std::integer_sequence<Int, Is...>>
{
  using type = std::integer_sequence<Int, (sizeof...(Is) - 1 - Is)...>;
};

} // namespace fmia::meta

export namespace fmia::meta {

// generate a sequence of integers of type T in [0, N) in a reversed order
//
// O(1) time complexity, assume `std::make_integer_sequence` will be optimized by compiler intrinsics, i.e. not a naive
// recursive implementation
template <std::integral T, T N>
using make_reversed_integer_sequence = make_reversed_integer_sequence_impl<std::make_integer_sequence<T, N>>::type;

template <std::size_t N>
using make_reversed_index_sequence = make_reversed_integer_sequence<std::size_t, N>;

template <typename... Ts>
using reversed_index_sequence_for = make_reversed_index_sequence<sizeof...(Ts)>;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename Int, Int, typename>
struct make_integer_sequence_of_range_impl;

template <typename Int, Int Begin, Int... Is>
struct make_integer_sequence_of_range_impl<Int, Begin, std::integer_sequence<Int, Is...>>
{
  using type = std::integer_sequence<Int, (Begin + Is)...>;
};

} // namespace fmia::meta

export namespace fmia::meta {

// generate a sequence of integers of type T in [Begin, End]
//
// O(1) time complexity, assume `std::make_integer_sequence` will be optimized by compiler intrinsics, i.e. not a naive
// recursive implementation
template <std::integral T, T Begin, T End>
  requires (Begin <= End)
using make_integer_sequence_of_range =
  make_integer_sequence_of_range_impl<T, Begin, std::make_integer_sequence<T, End - Begin + 1>>::type;

template <std::size_t Begin, std::size_t End>
using make_index_sequence_of_range = make_integer_sequence_of_range<std::size_t, Begin, End>;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename Int, Int, typename>
struct make_reversed_integer_sequence_of_range_impl;

template <typename Int, Int End, Int... Is>
struct make_reversed_integer_sequence_of_range_impl<Int, End, std::integer_sequence<Int, Is...>>
{
  using type = std::integer_sequence<Int, (End - Is)...>;
};

} // namespace fmia::meta

export namespace fmia::meta {

// generate a sequence of integers of type T in [Begin, End] in a reversed order
//
// O(1) time complexity, assume `std::make_integer_sequence` will be optimized by compiler intrinsics, i.e. not a naive
// recursive implementation
template <std::integral T, T Begin, T End>
  requires (Begin <= End)
using make_reversed_integer_sequence_of_range =
  make_reversed_integer_sequence_of_range_impl<T, End, std::make_integer_sequence<T, End - Begin + 1>>::type;

template <std::size_t Begin, std::size_t End>
using make_reversed_index_sequence_of_range = make_reversed_integer_sequence_of_range<std::size_t, Begin, End>;

// add an offset to all the integers in the given `std::integer_sequence`
//
// O(1) time complexity
template <std::integral T, T, typename>
struct shift_integer_sequence;

template <typename Int, Int Offset, Int... Is>
struct shift_integer_sequence<Int, Offset, std::integer_sequence<Int, Is...>>
{
  using type = std::integer_sequence<Int, (Is + Offset)...>;
};

template <typename Int, Int Offset, typename IntegerSequence>
using shift_integer_sequence_t = shift_integer_sequence<Int, Offset, IntegerSequence>::type;

// add an offset to all indices in the given `std::index_sequence`
//
// O(1) time complexity
template <std::size_t Offset, typename IndexSequence>
using shift_index_sequence = shift_integer_sequence<std::size_t, Offset, IndexSequence>;

template <std::size_t Offset, typename IndexSequence>
using shift_index_sequence_t = shift_index_sequence<Offset, IndexSequence>::type;

template <typename>
struct is_no_cv_no_duplication_integer_sequence : std::true_type
{
};

// O(nlog n) time complexity, where n is the size of the given integer sequence
template <typename Int, Int... Is>
struct is_no_cv_no_duplication_integer_sequence<std::integer_sequence<Int, Is...>>
{
  static constexpr bool value = [] consteval noexcept {
    std::array<std::size_t, sizeof...(Is)> I {Is...};
    std::ranges::sort(I);
    for (auto i = 1uz; i < I.size(); ++i)
      if (I[i - 1] == I[i])
        return false;

    return true;
  }();
};

template <typename T>
constexpr bool is_no_cv_no_duplication_integer_sequence_v = is_no_cv_no_duplication_integer_sequence<T>::value;

template <typename T>
using is_no_duplication_integer_sequence = is_no_cv_no_duplication_integer_sequence<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_no_duplication_integer_sequence_v = is_no_duplication_integer_sequence<T>::value;

} // export namespace fmia::meta

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

template <typename T>
concept multidimentional_cstyle_array = std::rank_v<std::remove_cvref_t<T>> > 1;

} // export namespace fmia::meta

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
