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

// general template metaprogramming utilities
export module aatk.meta;

import std;

namespace aatk::meta {

export template <typename T>
struct has_cv : std::bool_constant<std::is_const_v<T> || std::is_volatile_v<T>>
{
};

export template <typename T>
constexpr bool has_cv_v = has_cv<T>::value;

export template <typename T>
struct is_cv : std::bool_constant<std::is_const_v<T> && std::is_volatile_v<T>>
{
};

export template <typename T>
constexpr bool is_cv_v = is_cv<T>::value;

namespace detail {

template <typename TFrom, typename TTo, bool = std::is_const_v<TFrom>, bool = std::is_volatile_v<TFrom>>
struct claim_cv_selector;

// branch 1: has both cv qualifiers
template <typename TFrom, typename TTo>
struct claim_cv_selector<TFrom, TTo, true, true> : std::add_cv<TTo>
{
};

// branch 2: has only const qualifier
template <typename TFrom, typename TTo>
struct claim_cv_selector<TFrom, TTo, true, false> : std::add_const<TTo>
{
};

// branch 3: has only volatile qualifier
template <typename TFrom, typename TTo>
struct claim_cv_selector<TFrom, TTo, false, true> : std::add_volatile<TTo>
{
};

// branch 4: has no cv qualifiers
template <typename TFrom, typename TTo>
struct claim_cv_selector<TFrom, TTo, false, false>
{
  using type = TTo;
};

} // namespace detail

// extract the cv-qualifiers of TFrom and apply them to TTo
export template <typename TFrom, typename TTo>
using claim_cv = detail::claim_cv_selector<TFrom, TTo>;

export template <typename TFrom, typename TTo>
using claim_cv_t = claim_cv<TFrom, TTo>::type;

export template <typename T, typename U>
concept no_cvref_same_as = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

export template <typename T, typename U>
concept no_cvref_not_same_as = !no_cvref_same_as<T, U>;

export template <std::size_t N>
using index_constant = std::integral_constant<std::size_t, N>;

namespace detail {

template <typename TIntegerSequence>
struct make_reversed_integer_sequence_impl;

template <typename TInt, TInt... Is>
struct make_reversed_integer_sequence_impl<std::integer_sequence<TInt, Is...>>
{
  using type = std::integer_sequence<TInt, (sizeof...(Is) - 1 - Is)...>;
};

} // namespace detail

// generate a sequence of integers of type T in [0, N) in a reversed order
// O(1) time complexity, assume `std::make_integer_sequence` will be optimized by compiler intrinsics, i.e. not a naive recursive implementation
export template <std::integral T, T N>
using make_reversed_integer_sequence = detail::make_reversed_integer_sequence_impl<std::make_integer_sequence<T, N>>::type;

export template <std::size_t N>
using make_reversed_index_sequence = make_reversed_integer_sequence<std::size_t, N>;

export template <typename... Ts>
using reversed_index_sequence_for = make_reversed_index_sequence<sizeof...(Ts)>;

namespace detail {

template <typename TInt, TInt Begin, typename TIntegerSequence>
struct make_integer_sequence_of_range_impl;

template <typename TInt, TInt Begin, TInt... Is>
struct make_integer_sequence_of_range_impl<TInt, Begin, std::integer_sequence<TInt, Is...>>
{
  using type = std::integer_sequence<TInt, (Begin + Is)...>;
};

} // namespace detail

// generate a sequence of integers of type T in [Begin, End]
// O(1) time complexity, assume `std::make_integer_sequence` will be optimized by compiler intrinsics, i.e. not a naive recursive implementation
export template <std::integral T, T Begin, T End>
  requires (Begin <= End)
using make_integer_sequence_of_range = detail::make_integer_sequence_of_range_impl<T, Begin, std::make_integer_sequence<T, End - Begin + 1>>::type;

export template <std::size_t Begin, std::size_t End>
using make_index_sequence_of_range = make_integer_sequence_of_range<std::size_t, Begin, End>;

namespace detail {

template <typename TInt, TInt End, typename TIntegerSequence>
struct make_reversed_integer_sequence_of_range_impl;

template <typename TInt, TInt End, TInt... Is>
struct make_reversed_integer_sequence_of_range_impl<TInt, End, std::integer_sequence<TInt, Is...>>
{
  using type = std::integer_sequence<TInt, (End - Is)...>;
};

} // namespace detail

// generate a sequence of integers of type T in [Begin, End] in a reversed order
// O(1) time complexity, assume `std::make_integer_sequence` will be optimized by compiler intrinsics, i.e. not a naive recursive implementation
export template <std::integral T, T Begin, T End>
  requires (Begin <= End)
using make_reversed_integer_sequence_of_range = detail::make_reversed_integer_sequence_of_range_impl<T, End, std::make_integer_sequence<T, End - Begin + 1>>::type;

export template <std::size_t Begin, std::size_t End>
using make_reversed_index_sequence_of_range = make_reversed_integer_sequence_of_range<std::size_t, Begin, End>;

export template <typename T, typename... Us>
  requires (sizeof...(Us) > 0)
struct is_none_of : std::conjunction<is_none_of<T, Us>...>
{
};

export template <typename T, typename U>
struct is_none_of<T, U> : std::bool_constant<!std::same_as<T, U>>
{
};

export template <typename T, typename... Us>
constexpr bool is_none_of_v = is_none_of<T, Us...>::value;

export template <typename T, typename... Us>
  requires (sizeof...(Us) > 0)
struct is_any_of : std::negation<is_none_of<T, Us...>>
{
};

export template <typename T, typename... Us>
constexpr bool is_any_of_v = is_any_of<T, Us...>::value;

export template <typename...>
struct all_the_same;

export template <>
struct all_the_same<> : std::true_type
{
};

export template <typename T>
struct all_the_same<T> : std::true_type
{
};

export template <typename T0, typename T1, typename... Ts>
struct all_the_same<T0, T1, Ts...> : std::bool_constant<std::same_as<T0, T1> && all_the_same<T1, Ts...>::value>
{
};

export template <typename... Ts>
constexpr bool all_the_same_v = all_the_same<Ts...>::value;

export template <typename... Ts>
using type_list = std::tuple<Ts...>;

export using empty_type_list = type_list<>;

export template <typename>
struct is_no_cv_type_list : std::false_type
{
};

export template <typename... Ts>
struct is_no_cv_type_list<type_list<Ts...>> : std::true_type
{
};

export template <typename T>
constexpr bool is_no_cv_type_list_v = is_no_cv_type_list<T>::value;

export template <typename T>
using is_type_list = is_no_cv_type_list<std::remove_cv_t<T>>;

export template <typename T>
constexpr bool is_type_list_v = is_type_list<T>::value;

export template <typename T>
concept list_of_types = is_type_list_v<T>;

export template <typename T>
struct is_no_cv_empty_type_list : std::false_type
{
};

export template <>
struct is_no_cv_empty_type_list<empty_type_list> : std::true_type
{
};

export template <typename T>
constexpr bool is_no_cv_empty_type_list_v = is_no_cv_empty_type_list<T>::value;

export template <typename T>
using is_empty_type_list = is_no_cv_empty_type_list<std::remove_cv_t<T>>;

export template <typename T>
constexpr bool is_empty_type_list_v = is_empty_type_list<T>::value;

export template <typename T>
concept nonempty_list_of_types = list_of_types<T> && !is_empty_type_list_v<T>;

export template <typename, list_of_types>
struct has_none;

export template <typename T>
struct has_none<T, empty_type_list> : std::true_type
{
};

export template <typename T, typename... Us>
struct has_none<T, type_list<Us...>> : is_none_of<T, Us...>
{
};

export template <typename T, typename U>
constexpr bool has_none_v = has_none<T, U>::value;

export template <typename T, list_of_types U>
struct has_any : std::negation<has_none<T, U>>
{
};

export template <typename T, typename U>
constexpr bool has_any_v = has_any<T, U>::value;

// get the length of a type list
// O(1) time complexity
// name after Haskell Data.List length
export template <list_of_types>
struct length;

export template <typename... Ts>
struct length<type_list<Ts...>> : index_constant<sizeof...(Ts)>
{
};

export template <typename T>
constexpr std::size_t length_v = length<T>::value;

// get the nth type of a type list, index starts at 0
// O(1) time complexity, assume C++26 pack indexing has O(1) time complexity
export template <std::size_t Idx, list_of_types T>
  requires (Idx < length_v<T>)
struct nth;

export template <std::size_t Idx, typename... Ts>
struct nth<Idx, type_list<Ts...>>
{
  using type = Ts...[Idx];
};

export template <std::size_t Idx, typename T>
using nth_t = nth<Idx, T>::type;

// get the first type of a type list
// O(1) time complexity
// name after Haskell Data.List head
export template <nonempty_list_of_types T>
using head = nth<0, T>;

export template <typename T>
using head_t = head<T>::type;

// get the last type of a type list
// O(1) time complexity
// name after Haskell Data.List last
export template <nonempty_list_of_types T>
using last = nth<length_v<T> - 1, T>;

export template <typename T>
using last_t = last<T>::type;

// get a type list that has one element added to the beginning comparing to the given type list
// O(1) time complexity
// name after Haskell Data.List : operator (1 : [1, 2] --> [1, 1, 2])
export template <typename, list_of_types>
struct cons;

export template <typename T, typename... Ts>
struct cons<T, type_list<Ts...>>
{
  using type = type_list<T, Ts...>;
};

export template <typename T, typename U>
using cons_t = cons<T, U>::type;

// get a type list that has one element added to the end comparing to the given type list
// O(1) time complexity
export template <typename, list_of_types>
struct snoc;

export template <typename T, typename... Ts>
struct snoc<T, type_list<Ts...>>
{
  using type = type_list<Ts..., T>;
};

export template <typename T, typename U>
using snoc_t = snoc<T, U>::type;

namespace detail {

template <typename T, typename TIndexSequence>
struct repeat_impl;

template <typename T, std::size_t... Is>
struct repeat_impl<T, std::index_sequence<Is...>>
{
  using type = type_list<std::enable_if_t<(Is >= 0), T>...>;
};

} // namespace detail

// get a type list that contains N identical types
// O(1) time complexity
// name after Haskell Data.List repeat
export template <std::size_t N, typename T>
using repeat = detail::repeat_impl<T, std::make_index_sequence<N>>;

export template <std::size_t N, typename T>
using repeat_t = repeat<N, T>::type;

// get the concatenation of several type lists
// O(log n) time complexity, where n is the count of type lists to concatenate
// name after Haskell Data.List concat
export template <list_of_types...>
struct concat;

export template <typename... Ts>
struct concat<type_list<Ts...>>
{
  using type = type_list<Ts...>;
};

export template <typename... Ts, typename... Us>
struct concat<type_list<Ts...>, type_list<Us...>>
{
  using type = type_list<Ts..., Us...>;
};

namespace detail {

// devide and conquer for >= 3 type lists for better time complexity:
// 1. divide
// left half of the original type list is `concat_impl<BeginIdx, N / 2, Ts...>::type`
// right half of the original type list is `concat_impl<BeginIdx + N / 2, N - N / 2, Ts...>::type`
// 2. merge
// use the 2 lists specialization of `concat` to merge
template <std::size_t BeginIdx, std::size_t N, typename TTypeList>
struct concat_impl : concat<typename concat_impl<BeginIdx, N / 2, TTypeList>::type, typename concat_impl<BeginIdx + N / 2, N - N / 2, TTypeList>::type>
{
};

template <std::size_t BeginIdx, typename TTypeList>
struct concat_impl<BeginIdx, 1, TTypeList> : nth<BeginIdx, TTypeList>
{
};

template <std::size_t BeginIdx, typename TTypeList>
struct concat_impl<BeginIdx, 0, TTypeList>
{
  using type = empty_type_list;
};

} // namespace detail

export template <typename TTypeList, typename... TTypeLists>
struct concat<TTypeList, TTypeLists...> : detail::concat_impl<0, 1 + sizeof...(TTypeLists), type_list<TTypeList, TTypeLists...>>
{
};

export template <typename... TTypeLists>
using concat_t = concat<TTypeLists...>::type;

// get a type list that contains types whose indices are in the given `std::index_sequence`
// O(1) time complexity
export template <typename TIndexSequence, list_of_types>
struct select_by_index_sequence;

export template <std::size_t... Is, typename... Ts>
struct select_by_index_sequence<std::index_sequence<Is...>, type_list<Ts...>>
{
  using type = type_list<Ts...[Is]...>;
};

export template <typename TIndexSequence, typename TTypeList>
using select_by_index_sequence_t = select_by_index_sequence<TIndexSequence, TTypeList>::type;

// get a type list that is the reverse of the given type list
// O(1) time complexity
// name after Haskell Data.List reverse
export template <list_of_types T>
using reverse = select_by_index_sequence<make_reversed_index_sequence<length_v<T>>, T>;

export template <typename TTypeList>
using reverse_t = reverse<TTypeList>::type;

// get a type list with the first type removed comparing to the given type list
// O(1) time complexity
// name after Haskell Data.List tail
export template <nonempty_list_of_types>
struct tail;

export template <typename T, typename... Ts>
struct tail<type_list<T, Ts...>>
{
  using type = type_list<Ts...>;
};

export template <typename T>
using tail_t = tail<T>::type;

// get a type list with the last type removed comparing to the given type list
// O(1) time complexity
// name after Haskell Data.List init
export template <nonempty_list_of_types T>
using init = select_by_index_sequence<std::make_index_sequence<length_v<T> - 1>, T>;

export template <typename TTypeList>
using init_t = init<TTypeList>::type;

// get a type list that contains the first N types of the given type list
// O(1) time complexity
// name after Haskell Data.List take
export template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
using take = select_by_index_sequence<std::make_index_sequence<N>, T>;

export template <std::size_t N, typename TTypeList>
using take_t = take<N, TTypeList>::type;

// same as take, but take from the end
// O(1) time complexity
export template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
struct take_end : select_by_index_sequence<make_index_sequence_of_range<length_v<T> - N, length_v<T> - 1>, T>
{
};

export template <typename TTypeList>
struct take_end<0, TTypeList>
{
  using type = empty_type_list;
};

export template <std::size_t N, typename TTypeList>
using take_end_t = take_end<N, TTypeList>::type;

// get a type list with the first N types removed comparing to the given type list
// O(1) time complexity
// name after Haskell Data.List drop
export template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
using drop = take_end<length_v<T> - N, T>;

export template <std::size_t N, typename TTypeList>
using drop_t = drop<N, TTypeList>::type;

// same as drop, but drop from the end
// O(1) time complexity
export template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
using drop_end = take<length_v<T> - N, T>;

export template <std::size_t N, typename TTypeList>
using drop_end_t = drop_end<N, TTypeList>::type;

namespace detail {

template <template <typename...> typename, typename>
struct is_predicate_tester;

// clang-format off
template <template <typename...> typename TT, typename... Ts>
struct is_predicate_tester<TT, type_list<Ts...>> : std::bool_constant<requires { { TT<Ts...>::value } -> no_cvref_same_as<bool>; }>
{
};
// clang-format on

template <template <typename...> typename, typename>
struct is_predicate_impl;

template <template <typename...> typename TT, std::size_t... Is>
struct is_predicate_impl<TT, std::index_sequence<Is...>> : std::disjunction<is_predicate_tester<TT, repeat_t<Is + 1, void>>...>
{
};

} // namespace detail

export template <template <typename...> typename TT, std::size_t ArityLimit = 5>
  requires (ArityLimit > 0)
struct is_predicate : detail::is_predicate_impl<TT, std::make_index_sequence<ArityLimit>>
{
};

export template <template <typename...> typename TT, std::size_t ArityLimit = 5>
constexpr bool is_predicate_v = is_predicate<TT, ArityLimit>::value;

export template <template <typename...> typename TT>
concept predicate = is_predicate_v<TT>;

export template <template <typename...> typename TT>
struct template_wrapper
{
  template <typename... TArgs>
  using type = TT<TArgs...>;
};

export template <typename>
struct is_no_cv_template_wrapper : std::false_type
{
};

export template <template <typename...> typename TT>
struct is_no_cv_template_wrapper<template_wrapper<TT>> : std::true_type
{
};

export template <typename T>
constexpr bool is_no_cv_template_wrapper_v = is_no_cv_template_wrapper<T>::value;

export template <typename T>
using is_template_wrapper = is_no_cv_template_wrapper<std::remove_cv_t<T>>;

export template <typename T>
constexpr bool is_template_wrapper_v = is_template_wrapper<T>::value;

export template <typename T>
concept wrapped_template = is_template_wrapper_v<T>;

export template <typename T>
concept wrapped_predicate = wrapped_template<T> && predicate<T::template type>;

export template <wrapped_template T, typename... TArgs>
using invoke = T::template type<TArgs...>;

export template <typename TWrappedTemplate, typename... TArgs>
using invoke_t = invoke<TWrappedTemplate, TArgs...>::type;

export template <typename TWrappedTemplate, typename... TArgs>
constexpr auto invoke_v = invoke<TWrappedTemplate, TArgs...>::value;

namespace detail {

template <typename TWrappedPred, typename TRemainingTypeList, typename TTakenTypeList = empty_type_list>
struct take_while_impl;

template <typename TWrappedPred, typename TTakenTypeList>
struct take_while_impl<TWrappedPred, empty_type_list, TTakenTypeList>
{
  using type = TTakenTypeList;
};

template <typename TWrappedPred, typename TRemainingTypeList, typename TTakenTypeList>
struct take_while_impl_lazy_evaluation_helper;

template <typename TWrappedPred, typename T, typename... Ts, typename... TTaken>
struct take_while_impl_lazy_evaluation_helper<TWrappedPred, type_list<T, Ts...>, type_list<TTaken...>>
{
  // cannot use inheritance here, otherwise the evaluation is not lazy
  using type = take_while_impl<TWrappedPred, type_list<Ts...>, type_list<TTaken..., T>>::type;
};

template <typename TWrappedPred, typename T, typename... Ts, typename... TTaken>
struct take_while_impl<TWrappedPred, type_list<T, Ts...>, type_list<TTaken...>>
  : std::conditional_t<invoke_v<TWrappedPred, T>, take_while_impl_lazy_evaluation_helper<TWrappedPred, type_list<T, Ts...>, type_list<TTaken...>>, std::type_identity<type_list<TTaken...>>>
{
};

} // namespace detail

// get the longest prefix type list whose types all satisfy a given predicate
// O(k) time complexity, where k is the length of the longest prefix
// name after Haskell Data.List takeWhile
export template <template <typename> typename TTPred, list_of_types T>
  requires predicate<TTPred>
using take_while = detail::take_while_impl<template_wrapper<TTPred>, T>;

export template <template <typename> typename TTPred, typename TTypeList>
using take_while_t = take_while<TTPred, TTypeList>::type;

// get the longest suffix type list whose types all satisfy a given predicate
// O(k) time complexity, where k is the length of the longest suffix
export template <template <typename> typename TTPred, list_of_types T>
  requires predicate<TTPred>
struct take_while_end : reverse<take_while_t<TTPred, reverse_t<T>>>
{
};

export template <template <typename> typename TTPred, typename TTypeList>
using take_while_end_t = take_while_end<TTPred, TTypeList>::type;

namespace detail {

template <template <typename> typename, typename TTypeList>
struct drop_while_impl;

template <template <typename> typename TTPred>
struct drop_while_impl<TTPred, empty_type_list>
{
  using type = empty_type_list;
};

template <template <typename> typename, typename TTypeList>
struct drop_while_impl_lazy_evaluation_helper;

template <template <typename> typename TTPred, typename T, typename... Ts>
struct drop_while_impl_lazy_evaluation_helper<TTPred, type_list<T, Ts...>>
{
  // cannot use inheritance here, otherwise the evaluation is not lazy
  using type = drop_while_impl<TTPred, type_list<Ts...>>::type;
};

template <template <typename> typename TTPred, typename T, typename... Ts>
struct drop_while_impl<TTPred, type_list<T, Ts...>> : std::conditional_t<TTPred<T>::value, drop_while_impl_lazy_evaluation_helper<TTPred, type_list<T, Ts...>>, std::type_identity<type_list<T, Ts...>>>
{
};

} // namespace detail

// get a type list with a longest prefix type list removed, whose types all satisfy a given predicate
// O(k) time complexity, where k is the longest dropped prefix
// name after Haskell Data.List dropWhile
export template <template <typename> typename TTPred, list_of_types T>
  requires predicate<TTPred>
using drop_while = detail::drop_while_impl<TTPred, T>;

export template <template <typename> typename TTPred, typename TTypeList>
using drop_while_t = drop_while<TTPred, TTypeList>::type;

// get a type list with a longest suffix type list removed, whose types all satisfy a given predicate
// O(k) time complexity, where k is the length of the longest dropped suffix
// name after Haskell Data.List dropWhileEnd
export template <template <typename> typename TTPred, list_of_types T>
  requires predicate<TTPred>
struct drop_while_end : reverse<drop_while_t<TTPred, reverse_t<T>>>
{
};

export template <template <typename> typename TTPred, typename TTypeList>
using drop_while_end_t = drop_while_end<TTPred, TTypeList>::type;

// get a type list that contains all the types that satisfy a given predicate
// O(log n) time complexity, limited by `concat`, where n is the length of the given type list
// name after Haskell Data.List filter
export template <template <typename> typename TTPred, list_of_types>
  requires predicate<TTPred>
struct filter;

export template <template <typename> typename TTPred>
struct filter<TTPred, empty_type_list>
{
  using type = empty_type_list;
};

export template <template <typename> typename TTPred, typename... Ts>
struct filter<TTPred, type_list<Ts...>> : concat<std::conditional_t<TTPred<Ts>::value, type_list<Ts>, empty_type_list>...>
{
};

export template <template <typename> typename TTPred, typename TTypeList>
using filter_t = filter<TTPred, TTypeList>::type;

} // namespace aatk::meta
