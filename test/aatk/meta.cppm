// Copyright (C) 2026 atyxeut
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

export module test.aatk.meta;

import std;

import aatk.meta;

namespace test::aatk::meta {

using namespace ::aatk::meta;

export consteval void do_custom_integer_sequence_helpers_work() noexcept
{
  static_assert(std::same_as<make_reversed_integer_sequence<int, 8>, std::integer_sequence<int, 7, 6, 5, 4, 3, 2, 1, 0>> == true);
  static_assert(std::same_as<make_reversed_index_sequence<8>, std::index_sequence<7, 6, 5, 4, 3, 2, 1, 0>> == true);
  static_assert(std::same_as<reversed_index_sequence_for<int, int, int, int, int>, std::index_sequence<4, 3, 2, 1, 0>> == true);

  static_assert(std::same_as<make_integer_sequence_of_range<int, 3, 7>, std::integer_sequence<int, 3, 4, 5, 6, 7>> == true);
  static_assert(std::same_as<make_index_sequence_of_range<3, 10>, std::index_sequence<3, 4, 5, 6, 7, 8, 9, 10>> == true);
  static_assert(std::same_as<make_reversed_integer_sequence_of_range<int, 3, 7>, std::integer_sequence<int, 7, 6, 5, 4, 3>> == true);
  static_assert(std::same_as<make_reversed_index_sequence_of_range<3, 10>, std::index_sequence<10, 9, 8, 7, 6, 5, 4, 3>> == true);

  static_assert(std::same_as<shift_integer_sequence_t<int, 10, std::integer_sequence<int, 1, 2, 3, 4>>, std::integer_sequence<int, 11, 12, 13, 14>>);
  static_assert(std::same_as<shift_index_sequence_t<50, std::index_sequence<1, 2, 3, 4>>, std::index_sequence<51, 52, 53, 54>>);
}

export consteval void does_is_no_duplication_integer_sequence_work() noexcept
{
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 1, 3>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 3, 4, 4, 6>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 3, 4, 5, 6>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 1, 2, 1, 2, 1>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 3, 4, 3, 3, 3>> == false);

  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 3>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 1, 3>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 3, 4, 4, 6>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 3, 4, 5, 6>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 1, 2, 1, 2, 1>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 3, 4, 3, 3, 3>> == false);
}

export consteval void does_is_any_of_work() noexcept
{
  static_assert(is_any_of_v<int> == false);
  static_assert(is_any_of_v<int, int> == true);
  static_assert(is_any_of_v<int, double> == false);
  static_assert(is_any_of_v<int, double, long double, char, const char, float, std::string, const int> == false);
  static_assert(is_any_of_v<int, double, long double, char, const char, int, std::string> == true);
}

export consteval void does_is_none_of_work() noexcept
{
  static_assert(is_none_of_v<int> == true);
  static_assert(is_none_of_v<int, int> == false);
  static_assert(is_none_of_v<int, double> == true);
  static_assert(is_none_of_v<int, double, long double, char, const char, float, std::string, const int> == true);
  static_assert(is_none_of_v<int, double, long double, char, const char, int, std::string> == false);
}

export consteval void does_all_the_same_work() noexcept
{
  static_assert(all_the_same_v<> == true);
  static_assert(all_the_same_v<int, int> == true);
  static_assert(all_the_same_v<int, double> == false);
  static_assert(all_the_same_v<int, int, int, int, int, const char, float, std::string> == false);
  static_assert(all_the_same_v<int, int, int, int, int, int, int, int> == true);
}

using type_list_1 = type_list<double, float, std::vector<int>, long long>;
using type_list_2 = type_list<std::string, unsigned, const volatile bool, bool>;
using type_list_3 = type_list<int, char, const int, volatile char>;
using type_list_4 = type_list<int, int, int, int, int, int, int>;
using type_list_5 = type_list<const float, float, double, int, const double, char, std::byte, volatile unsigned long long, volatile void, char, unsigned char, signed char, short, void, const void>;
using type_list_6 = type_list<const float, float, double, int, const double, std::byte, volatile unsigned long long, volatile void, unsigned char, signed char, short, void, const void>;

using indexed_type_list_1 = indexed_type_list<std::make_index_sequence<length_v<type_list_1>>, type_list_1>;
using indexed_type_list_2 = indexed_type_list<std::make_index_sequence<length_v<type_list_2>>, type_list_2>;
using indexed_type_list_3 = indexed_type_list<std::make_index_sequence<length_v<type_list_3>>, type_list_3>;
using indexed_type_list_4 = indexed_type_list<std::make_index_sequence<length_v<type_list_4>>, type_list_4>;
using indexed_type_list_5 = indexed_type_list<std::make_index_sequence<length_v<type_list_5>>, type_list_5>;
using indexed_type_list_6 = indexed_type_list<std::make_index_sequence<length_v<type_list_6>>, type_list_6>;

export consteval void does_lookup_work() noexcept
{
  static_assert(std::same_as<lookup_t<2, indexed_type_list_1>, indexed_type<2, std::vector<int>>>);
  static_assert(std::same_as<lookup_t<0, indexed_type_list_2>, indexed_type<0, std::string>>);
  static_assert(std::same_as<lookup_t<1, indexed_type_list_3>, indexed_type<1, char>>);
  static_assert(std::same_as<lookup_t<length_v<indexed_type_list_6> - 1, indexed_type_list_6>, indexed_type<length_v<indexed_type_list_6> - 1, const void>>);
}

export consteval void does_replicate_work() noexcept
{
  using replicateed_0_time_type_list = empty_type_list;
  static_assert(std::same_as<replicate_t<0, void>, replicateed_0_time_type_list>);

  using int_replicateed_5_times_type_list = type_list<int, int, int, int, int>;
  static_assert(std::same_as<replicate_t<5, int>, int_replicateed_5_times_type_list>);
}

namespace detail {

template <list_of_types>
struct huge_concat_test_helper;

template <list_of_types... Ts>
struct huge_concat_test_helper<type_list<Ts...>> : concat<Ts...>
{
};

template <list_of_types T>
using huge_concat_test_helper_t = huge_concat_test_helper<T>::type;

} // namespace detail

export consteval void does_concat_work() noexcept
{
  static_assert(std::same_as<concat_t<type_list_1>, type_list_1>);
  static_assert(std::same_as<concat_t<type_list_2>, type_list_2>);

  using concatenation_of_1_2 = type_list<double, float, std::vector<int>, long long, std::string, unsigned, const volatile bool, bool>;
  static_assert(std::same_as<concat_t<type_list_1, type_list_2>, concatenation_of_1_2>);

  using concatenation_of_1_2_3 = type_list<double, float, std::vector<int>, long long, std::string, unsigned, const volatile bool, bool, int, char, const int, volatile char>;
  static_assert(std::same_as<concat_t<type_list_1, type_list_2, type_list_3>, concatenation_of_1_2_3>);

  using concatenation_of_1_2_3_4 = type_list<double, float, std::vector<int>, long long, std::string, unsigned, const volatile bool, bool, int, char, const int, volatile char, int, int, int, int, int, int, int>;
  static_assert(std::same_as<concat_t<type_list_1, type_list_2, type_list_3, type_list_4>, concatenation_of_1_2_3_4>);

  // test `replicate` complexity, length_v<type_list_4> * 1500 recursion depth (for a O(N) recursive implementation) will make the compiler crash by default
  using huge_concatenation_before = replicate_t<1500, type_list_4>;
  using huge_concatenation_after = replicate_t<length_v<type_list_4> * 1500, int>;

  // test `concat` complexity, 1500 recursion depth (for a O(n) recursive implementation) will make the compiler crash by default
  static_assert(std::same_as<detail::huge_concat_test_helper_t<huge_concatenation_before>, huge_concatenation_after>);
}

export consteval void does_reverse_work() noexcept
{
  using reversed_empty_type_list = empty_type_list;
  static_assert(std::same_as<reverse_t<empty_type_list>, reversed_empty_type_list>);

  using reversed_type_list_of_1 = type_list<long long, std::vector<int>, float, double>;
  static_assert(std::same_as<reverse_t<type_list_1>, reversed_type_list_of_1>);

  using reversed_type_list_of_2 = type_list<bool, const volatile bool, unsigned, std::string>;
  static_assert(std::same_as<reverse_t<type_list_2>, reversed_type_list_of_2>);
}

export consteval void does_init_work() noexcept
{
  // force a template substitution to make `requires` work
  auto test_empty_list = []<typename T> consteval noexcept
  {
    return requires { typename init_t<T>; };
  };
  static_assert(test_empty_list.operator ()<empty_type_list>() == false);

  using init_type_list_of_1 = type_list<double, float, std::vector<int>>;
  static_assert(std::same_as<init_t<type_list_1>, init_type_list_of_1> == true);

  using init_type_list_of_2 = type_list<std::string, unsigned, const volatile bool>;
  static_assert(std::same_as<init_t<type_list_2>, init_type_list_of_2> == true);
}

export consteval void does_take_work() noexcept
{
  using take_0_type_list_of_empty = empty_type_list;
  static_assert(std::same_as<take_t<0, empty_type_list>, take_0_type_list_of_empty>);

  using take_0_type_list_of_nonempty = empty_type_list;
  static_assert(std::same_as<take_t<0, type_list_1>, take_0_type_list_of_nonempty>);

  using take_3_type_list_of_1 = type_list<double, float, std::vector<int>>;
  static_assert(std::same_as<take_t<3, type_list_1>, take_3_type_list_of_1>);

  using take_2_type_list_of_2 = type_list<std::string, unsigned>;
  static_assert(std::same_as<take_t<2, type_list_2>, take_2_type_list_of_2>);

  using take_6_type_list_of_5 = type_list<const float, float, double, int, const double, char>;
  static_assert(std::same_as<take_t<6, type_list_5>, take_6_type_list_of_5>);
}

export consteval void does_take_end_work() noexcept
{
  using take_0_type_list_of_empty_from_end = empty_type_list;
  static_assert(std::same_as<take_end_t<0, empty_type_list>, take_0_type_list_of_empty_from_end>);

  using take_0_type_list_of_nonempty_from_end = empty_type_list;
  static_assert(std::same_as<take_end_t<0, type_list_1>, take_0_type_list_of_nonempty_from_end>);

  using take_3_type_list_of_1_from_end = type_list<float, std::vector<int>, long long>;
  static_assert(std::same_as<take_end_t<3, type_list_1>, take_3_type_list_of_1_from_end>);

  using take_2_type_list_of_2_from_end = type_list<const volatile bool, bool>;
  static_assert(std::same_as<take_end_t<2, type_list_2>, take_2_type_list_of_2_from_end>);

  using take_6_type_list_of_5_from_end = type_list<char, unsigned char, signed char, short, void, const void>;
  static_assert(std::same_as<take_end_t<6, type_list_5>, take_6_type_list_of_5_from_end>);
}

export consteval void does_drop_work() noexcept
{
  using dropped_0_type_list_of_empty = empty_type_list;
  static_assert(std::same_as<drop_t<0, empty_type_list>, dropped_0_type_list_of_empty>);

  using dropped_0_type_list_of_1 = type_list<double, float, std::vector<int>, long long>;
  static_assert(std::same_as<drop_t<0, type_list_1>, dropped_0_type_list_of_1>);

  using dropped_0_type_list_of_2 = type_list<std::string, unsigned, const volatile bool, bool>;
  static_assert(std::same_as<drop_t<0, type_list_2>, dropped_0_type_list_of_2>);

  using dropped_3_type_list_of_1 = type_list<long long>;
  static_assert(std::same_as<drop_t<3, type_list_1>, dropped_3_type_list_of_1>);

  using dropped_3_type_list_of_2 = type_list<bool>;
  static_assert(std::same_as<drop_t<3, type_list_2>, dropped_3_type_list_of_2>);
}

export consteval void does_drop_end_work() noexcept
{
  using dropped_0_type_list_of_empty_from_end = empty_type_list;
  static_assert(std::same_as<drop_end_t<0, empty_type_list>, dropped_0_type_list_of_empty_from_end>);

  using dropped_0_type_list_of_1_from_end = type_list<double, float, std::vector<int>, long long>;
  static_assert(std::same_as<drop_end_t<0, type_list_1>, dropped_0_type_list_of_1_from_end>);

  using dropped_0_type_list_of_2_from_end = type_list<std::string, unsigned, const volatile bool, bool>;
  static_assert(std::same_as<drop_end_t<0, type_list_2>, dropped_0_type_list_of_2_from_end>);

  using dropped_3_type_list_of_1_from_end = type_list<double>;
  static_assert(std::same_as<drop_end_t<3, type_list_1>, dropped_3_type_list_of_1_from_end>);

  using dropped_3_type_list_of_2_from_end = type_list<std::string>;
  static_assert(std::same_as<drop_end_t<3, type_list_2>, dropped_3_type_list_of_2_from_end>);

  using dropped_2_type_list_of_3_from_end = type_list<int, char>;
  static_assert(std::same_as<drop_end_t<2, type_list_3>, dropped_2_type_list_of_3_from_end>);
}

export consteval void does_is_predicate_work() noexcept
{
  // test for templates that accept 1 template parameter
  static_assert(is_predicate_v<std::is_integral> == true);

  // test for templates that accept more than 1 (but fixed) template parameter
  static_assert(is_predicate_v<std::is_same> == true);

  // test for templates that accept variadic template parameters
  static_assert(is_predicate_v<std::is_constructible> == true);

  // test for templates whose `value` member does not have type bool
  static_assert(is_predicate_v<std::tuple_size> == false);

  // test for templates that do not have a `value` member
  static_assert(is_predicate_v<std::add_const> == false);

  // test if `wrapped_predicate` concept works
  static_assert(predicate<std::is_integral> == true);
  static_assert(wrapped_predicate<template_wrapper<std::is_integral>> == true);
  static_assert(predicate<std::is_same> == true);
  static_assert(wrapped_predicate<template_wrapper<std::is_same>> == true);
  static_assert(predicate<std::add_const> == false);
  static_assert(wrapped_predicate<template_wrapper<std::add_const>> == false);
}

export consteval void does_take_while_work() noexcept
{
  using take_while_type_list_of_empty_list = empty_type_list;
  static_assert(std::same_as<take_while_t<std::is_integral, empty_type_list>, take_while_type_list_of_empty_list>);

  using take_while_type_list_of_1 = empty_type_list;
  static_assert(std::same_as<take_while_t<std::is_integral, type_list_1>, take_while_type_list_of_1>);

  using take_while_type_list_of_2 = empty_type_list;
  static_assert(std::same_as<take_while_t<std::is_integral, type_list_2>, take_while_type_list_of_2>);

  using take_while_type_list_of_3 = type_list<int, char, const int, volatile char>;
  static_assert(std::same_as<take_while_t<std::is_integral, type_list_3>, take_while_type_list_of_3>);

  using take_while_type_list_of_4 = empty_type_list;
  static_assert(std::same_as<take_while_t<std::is_floating_point, type_list_4>, take_while_type_list_of_4>);

  using take_while_type_list_of_5_1 = empty_type_list;
  static_assert(std::same_as<take_while_t<std::is_void, type_list_5>, take_while_type_list_of_5_1>);

  using take_while_type_list_of_5_2 = type_list<const float, float, double>;
  static_assert(std::same_as<take_while_t<std::is_floating_point, type_list_5>, take_while_type_list_of_5_2>);
}

export consteval void does_take_while_end_work() noexcept
{
  using take_while_type_list_of_empty_list_from_end = empty_type_list;
  static_assert(std::same_as<take_while_end_t<std::is_integral, empty_type_list>, take_while_type_list_of_empty_list_from_end>);

  using take_while_type_list_of_1_from_end = type_list<long long>;
  static_assert(std::same_as<take_while_end_t<std::is_integral, type_list_1>, take_while_type_list_of_1_from_end>);

  using take_while_type_list_of_2_from_end = type_list<unsigned, const volatile bool, bool>;
  static_assert(std::same_as<take_while_end_t<std::is_integral, type_list_2>, take_while_type_list_of_2_from_end>);

  using take_while_type_list_of_3_from_end = type_list<int, char, const int, volatile char>;
  static_assert(std::same_as<take_while_end_t<std::is_integral, type_list_3>, take_while_type_list_of_3_from_end>);

  using take_while_type_list_of_4_from_end = empty_type_list;
  static_assert(std::same_as<take_while_end_t<std::is_floating_point, type_list_4>, take_while_type_list_of_4_from_end>);

  using take_while_type_list_of_5_1_from_end = type_list<void, const void>;
  static_assert(std::same_as<take_while_end_t<std::is_void, type_list_5>, take_while_type_list_of_5_1_from_end>);

  using take_while_type_list_of_5_2_from_end = empty_type_list;
  static_assert(std::same_as<take_while_end_t<std::is_floating_point, type_list_5>, take_while_type_list_of_5_2_from_end>);
}

export consteval void does_drop_while_work() noexcept
{
  using drop_while_type_list_of_1 = type_list<double, float, std::vector<int>, long long>;
  static_assert(std::same_as<drop_while_t<std::is_integral, type_list_1>, drop_while_type_list_of_1>);

  using drop_while_type_list_of_2 = type_list<std::string, unsigned, const volatile bool, bool>;
  static_assert(std::same_as<drop_while_t<std::is_integral, type_list_2>, drop_while_type_list_of_2>);

  using drop_while_type_list_of_3 = empty_type_list;
  static_assert(std::same_as<drop_while_t<std::is_integral, type_list_3>, drop_while_type_list_of_3>);

  using drop_while_type_list_of_4 = type_list<int, int, int, int, int, int, int>;
  static_assert(std::same_as<drop_while_t<std::is_void, type_list_4>, drop_while_type_list_of_4>);

  using drop_while_type_list_of_5 = type_list<int, const double, char, std::byte, volatile unsigned long long, volatile void, char, unsigned char, signed char, short, void, const void>;
  static_assert(std::same_as<drop_while_t<std::is_floating_point, type_list_5>, drop_while_type_list_of_5>);
}

export consteval void does_drop_while_end_work() noexcept
{
  using drop_while_type_list_of_1_from_end = type_list<double, float, std::vector<int>>;
  static_assert(std::same_as<drop_while_end_t<std::is_integral, type_list_1>, drop_while_type_list_of_1_from_end>);

  using drop_while_type_list_of_2_from_end = type_list<std::string>;
  static_assert(std::same_as<drop_while_end_t<std::is_integral, type_list_2>, drop_while_type_list_of_2_from_end>);

  using drop_while_type_list_of_3_from_end = empty_type_list;
  static_assert(std::same_as<drop_while_end_t<std::is_integral, type_list_3>, drop_while_type_list_of_3_from_end>);

  using drop_while_type_list_of_4_from_end = type_list<int, int, int, int, int, int, int>;
  static_assert(std::same_as<drop_while_end_t<std::is_void, type_list_4>, drop_while_type_list_of_4_from_end>);

  using drop_while_type_list_of_5_from_end = type_list<const float, float, double, int, const double, char, std::byte, volatile unsigned long long, volatile void, char, unsigned char, signed char, short, void, const void>;
  static_assert(std::same_as<drop_while_end_t<std::is_floating_point, type_list_5>, drop_while_type_list_of_5_from_end>);
}

export consteval void does_filter_work() noexcept
{
  using filtered_type_list_of_5_1 = type_list<int, char, volatile unsigned long long, char, unsigned char, signed char, short>;
  static_assert(std::same_as<filter_t<std::is_integral, type_list_5>, filtered_type_list_of_5_1>);

  using filtered_type_list_of_5_2 = type_list<const float, float, double, const double>;
  static_assert(std::same_as<filter_t<std::is_floating_point, type_list_5>, filtered_type_list_of_5_2>);

  using filtered_type_list_of_5_3 = type_list<volatile void, void, const void>;
  static_assert(std::same_as<filter_t<std::is_void, type_list_5>, filtered_type_list_of_5_3>);

  using filtered_type_list_of_5_4 = type_list<volatile unsigned long long, unsigned char>;
  static_assert(std::same_as<filter_t<std::is_unsigned, type_list_5>, filtered_type_list_of_5_4>);

  using filtered_type_list_of_6 = type_list<const float, float, double, int, const double, signed char, short>;
  static_assert(std::same_as<filter_t<std::is_signed, type_list_6>, filtered_type_list_of_6>);
}

export consteval void does_keep_work() noexcept
{
  using keep_only_int_type_list_of_empty = empty_type_list;
  static_assert(std::same_as<keep_t<int, empty_type_list>, keep_only_int_type_list_of_empty>);

  using keep_only_int_type_list_of_1 = empty_type_list;
  static_assert(std::same_as<keep_t<int, type_list_1>, keep_only_int_type_list_of_1>);

  using keep_only_int_type_list_of_3 = type_list<int>;
  static_assert(std::same_as<keep_t<int, type_list_3>, keep_only_int_type_list_of_3>);

  using keep_only_int_type_list_of_4 = type_list<int, int, int, int, int, int, int>;
  static_assert(std::same_as<keep_t<int, type_list_4>, keep_only_int_type_list_of_4>);

  using keep_only_char_type_list_of_5 = type_list<char, char>;
  static_assert(std::same_as<keep_t<char, type_list_5>, keep_only_char_type_list_of_5>);
}

export consteval void does_remove_work() noexcept
{
  using remove_all_int_type_list_of_empty = empty_type_list;
  static_assert(std::same_as<remove_t<int, empty_type_list>, remove_all_int_type_list_of_empty>);

  using remove_all_int_type_list_of_1 = type_list<double, float, std::vector<int>, long long>;
  static_assert(std::same_as<remove_t<int, type_list_1>, remove_all_int_type_list_of_1>);

  using remove_all_int_type_list_of_3 = type_list<char, const int, volatile char>;
  static_assert(std::same_as<remove_t<int, type_list_3>, remove_all_int_type_list_of_3>);

  using remove_all_int_type_list_of_4 = empty_type_list;
  static_assert(std::same_as<remove_t<int, type_list_4>, remove_all_int_type_list_of_4>);

  using remove_all_char_type_list_of_5 = type_list<const float, float, double, int, const double, std::byte, volatile unsigned long long, volatile void, unsigned char, signed char, short, void, const void>;
  static_assert(std::same_as<remove_t<char, type_list_5>, remove_all_char_type_list_of_5>);
}

export consteval void does_transform_work() noexcept
{
  using type_list_1_after_add_const = type_list<const double, const float, const std::vector<int>, const long long>;
  static_assert(std::same_as<transform_t<std::add_const, type_list_1>, type_list_1_after_add_const>);

  using type_list_4_after_make_unsigned = type_list<unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>;
  static_assert(std::same_as<transform_t<std::make_unsigned, type_list_4>, type_list_4_after_make_unsigned>);
}

export consteval void does_any_work() noexcept
{
  static_assert(any_v<std::is_integral, empty_type_list> == false);
  static_assert(any_v<std::is_integral, type_list_1> == true);
  static_assert(any_v<std::is_floating_point, type_list_2> == false);
  static_assert(any_v<std::is_floating_point, type_list_3> == false);
  static_assert(any_v<std::is_unsigned, type_list_4> == false);
  static_assert(any_v<std::is_unsigned, type_list_5> == true);
}

export consteval void does_none_work() noexcept
{
  static_assert(none_v<std::is_integral, empty_type_list> == true);
  static_assert(none_v<std::is_integral, type_list_1> == false);
  static_assert(none_v<std::is_floating_point, type_list_2> == true);
  static_assert(none_v<std::is_floating_point, type_list_3> == true);
  static_assert(none_v<std::is_unsigned, type_list_4> == true);
  static_assert(none_v<std::is_unsigned, type_list_5> == false);
}

export consteval void does_all_work() noexcept
{
  static_assert(all_v<std::is_integral, empty_type_list> == false);
  static_assert(all_v<std::is_integral, type_list_1> == false);
  static_assert(all_v<std::is_floating_point, type_list_2> == false);
  static_assert(all_v<std::is_floating_point, type_list_3> == false);
  static_assert(all_v<std::is_unsigned, type_list_4> == false);
  static_assert(all_v<std::is_signed, type_list_4> == true);
}

export consteval void does_has_any_work() noexcept
{
  static_assert(has_any_v<double, empty_type_list> == false);
  static_assert(has_any_v<double, type_list_1> == true);
  static_assert(has_any_v<int, type_list_2> == false);
  static_assert(has_any_v<double, indexed_type_list_1> == true);
  static_assert(has_any_v<int, indexed_type_list_2> == false);
}

export consteval void does_has_none_work() noexcept
{
  static_assert(has_none_v<double, empty_type_list> == true);
  static_assert(has_none_v<int, type_list_1> == true);
  static_assert(has_none_v<int, type_list_3> == false);
  static_assert(has_none_v<int, indexed_type_list_1> == true);
  static_assert(has_none_v<int, indexed_type_list_3> == false);
}

} // namespace test::aatk::meta
