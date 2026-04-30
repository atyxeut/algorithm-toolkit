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

export module fmia.meta.type_list;

import std;

import fmia.meta.core;

export namespace fmia::meta {

template <typename T, typename... Us>
struct is_any_of : std::disjunction<is_any_of<T, Us>...>
{
};

template <typename T, typename U>
struct is_any_of<T, U> : std::bool_constant<std::same_as<T, U>>
{
};

template <typename T, typename... Us>
constexpr bool is_any_of_v = is_any_of<T, Us...>::value;

template <typename T, typename... Us>
using is_none_of = std::negation<is_any_of<T, Us...>>;

template <typename T, typename... Us>
constexpr bool is_none_of_v = is_none_of<T, Us...>::value;

template <typename...>
struct all_the_same : std::true_type
{
};

template <typename T, typename... Us>
struct all_the_same<T, Us...> : std::conjunction<all_the_same<T, Us>...>
{
};

template <typename T, typename U>
struct all_the_same<T, U> : std::bool_constant<std::same_as<T, U>>
{
};

template <typename... Ts>
constexpr bool all_the_same_v = all_the_same<Ts...>::value;

} // namespace fmia::meta

export namespace fmia::meta {

template <typename...>
struct type_list
{
};

template <typename>
struct is_no_cv_type_list : std::false_type
{
};

template <typename... Ts>
struct is_no_cv_type_list<type_list<Ts...>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_type_list_v = is_no_cv_type_list<T>::value;

template <typename T>
using is_type_list = is_no_cv_type_list<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_type_list_v = is_type_list<T>::value;

using empty_type_list = type_list<>;

template <typename>
struct is_no_cv_empty_type_list : std::false_type
{
};

template <>
struct is_no_cv_empty_type_list<empty_type_list> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_empty_type_list_v = is_no_cv_empty_type_list<T>::value;

template <typename T>
using is_empty_type_list = is_no_cv_empty_type_list<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_empty_type_list_v = is_empty_type_list<T>::value;

template <std::size_t I, typename T>
struct indexed_type
{
  static constexpr auto idx = I;
  using type = T;
};

template <std::size_t I, typename T>
constexpr auto indexed_type_v = indexed_type<I, T>::idx;

template <std::size_t I, typename T>
using indexed_type_t = indexed_type<I, T>::type;

template <typename>
struct is_no_cv_indexed_type : std::false_type
{
};

template <std::size_t I, typename T>
struct is_no_cv_indexed_type<indexed_type<I, T>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_indexed_type_v = is_no_cv_indexed_type<T>::value;

template <typename T>
using is_indexed_type = is_no_cv_indexed_type<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_indexed_type_v = is_indexed_type<T>::value;

template <typename, typename>
struct indexed_type_list;

template <std::size_t... Is, typename... Ts>
struct indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>> : indexed_type<Is, Ts>...
{
  using indices = std::index_sequence<Is...>;
  using types = type_list<Ts...>;
};

template <typename>
struct is_no_cv_indexed_type_list : std::false_type
{
};

template <std::size_t... Is, typename... Ts>
struct is_no_cv_indexed_type_list<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_indexed_type_list_v = is_no_cv_indexed_type_list<T>::value;

template <typename T>
using is_indexed_type_list = is_no_cv_indexed_type_list<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_indexed_type_list_v = is_indexed_type_list<T>::value;

using empty_indexed_type_list = indexed_type_list<std::index_sequence<>, type_list<>>;

template <typename>
struct is_no_cv_empty_indexed_type_list : std::false_type
{
};

template <>
struct is_no_cv_empty_indexed_type_list<empty_indexed_type_list> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_empty_indexed_type_list_v = is_no_cv_empty_indexed_type_list<T>::value;

template <typename T>
using is_empty_indexed_type_list = is_no_cv_empty_indexed_type_list<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_empty_indexed_type_list_v = is_empty_indexed_type_list<T>::value;

template <typename T>
concept list_of_types = is_type_list_v<T> || is_indexed_type_list_v<T>;

template <typename T>
concept empty_list_of_types = is_empty_type_list_v<T> || is_empty_indexed_type_list_v<T>;

template <typename T>
concept nonempty_list_of_types = list_of_types<T> && !empty_list_of_types<T>;

// get the length of a type list
//
// name after Haskell Data.List length
//
// O(1) time complexity
template <typename>
struct length;

template <typename... Ts>
struct length<type_list<Ts...>> : index_constant<sizeof...(Ts)>
{
};

template <std::size_t... Is, typename... Ts>
struct length<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>> : index_constant<sizeof...(Ts)>
{
};

template <typename AnyTypeList>
constexpr std::size_t length_v = length<AnyTypeList>::value;

// get the nth type of a type list, index starts at 0
//
// O(1) time complexity, assume C++26 pack indexing has O(1) time complexity
template <std::size_t, typename>
struct nth;

template <std::size_t Idx, typename... Ts>
struct nth<Idx, type_list<Ts...>>
{
  using type = Ts...[Idx];
};

template <std::size_t Idx, std::size_t... Is, typename... Ts>
struct nth<Idx, indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>>
{
  using type = indexed_type<Is...[Idx], Ts...[Idx]>;
};

template <std::size_t Idx, typename AnyTypeList>
using nth_t = nth<Idx, AnyTypeList>::type;

// get the first type of a type list
//
// name after Haskell Data.List head
//
// O(1) time complexity
template <nonempty_list_of_types T>
using head = nth<0, T>;

template <typename T>
using head_t = head<T>::type;

// get the last type of a type list
//
// name after Haskell Data.List last
//
// O(1) time complexity
template <nonempty_list_of_types T>
using last = nth<length_v<T> - 1, T>;

template <typename T>
using last_t = last<T>::type;

} // namespace fmia::meta

namespace fmia::meta {

template <std::size_t I, typename T>
indexed_type<I, T> lookup_indexed_type_helper(indexed_type<I, T>);

} // namespace fmia::meta

export namespace fmia::meta {

// get the corresponding indexed type of a indexed_type_list with a given index
//
// name after Haskell Data.List lookup
//
// O(1) time complexity
template <std::size_t I, typename IndexedTypeList>
  requires is_indexed_type_list_v<IndexedTypeList>
struct lookup
{
  using type = decltype(lookup_indexed_type_helper<I>(std::declval<IndexedTypeList>()));
};

template <std::size_t I, typename IndexedTypeList>
using lookup_t = lookup<I, IndexedTypeList>::type;

// get a type list that has one element added to the beginning comparing to the given type list
//
// name after Haskell Data.List : operator (1 : [1, 2] --> [1, 1, 2])
//
// O(1) time complexity for type_list
// O(n) time complexity for indexed_type_list (because of the validation for indices), where n is the length of the
// given type list
template <typename, typename>
struct cons;

template <typename T, typename... Ts>
struct cons<T, type_list<Ts...>>
{
  using type = type_list<T, Ts...>;
};

template <typename T, std::size_t... Is, typename... Ts>
  requires (is_indexed_type_v<T> && ((T::idx != Is) && ...))
struct cons<T, indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>>
{
  using type = indexed_type_list<std::index_sequence<T::idx, Is...>, type_list<typename T::type, Ts...>>;
};

template <typename T, typename AnyTypeList>
using cons_t = cons<T, AnyTypeList>::type;

// get a type list that has one element added to the end comparing to the given type list
//
// O(1) time complexity for type_list
// O(n) time complexity for indexed_type_list (because of the validation for indices), where n is the length of the
// given type list
template <typename, typename>
struct snoc;

template <typename T, typename... Ts>
struct snoc<T, type_list<Ts...>>
{
  using type = type_list<Ts..., T>;
};

template <typename T, std::size_t... Is, typename... Ts>
  requires (is_indexed_type_v<T> && ((T::idx != Is) && ...))
struct snoc<T, indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>>
{
  using type = indexed_type_list<std::index_sequence<Is..., T::idx>, type_list<Ts..., typename T::type>>;
};

template <typename T, typename AnyTypeList>
using snoc_t = snoc<T, AnyTypeList>::type;

} // namespace fmia::meta

namespace fmia::meta {

template <typename, typename>
struct replicate_impl;

template <typename T, std::size_t... Is>
struct replicate_impl<T, std::index_sequence<Is...>>
{
  using type = type_list<std::enable_if_t<(Is >= 0), T>...>;
};

} // namespace fmia::meta

export namespace fmia::meta {

// get a type list that contains N identical types
//
// name after Haskell Data.List replicate
//
// O(1) time complexity
template <std::size_t N, typename T>
using replicate = replicate_impl<T, std::make_index_sequence<N>>;

template <std::size_t N, typename T>
using replicate_t = replicate<N, T>::type;

template <template <typename...> typename T, typename... BoundArgs>
struct bind_front
{
  template <typename... CallArgs>
  using type = T<BoundArgs..., CallArgs...>;
};

template <template <typename...> typename T, typename... BoundArgs>
struct bind_back
{
  template <typename... CallArgs>
  using type = T<CallArgs..., BoundArgs...>;
};

template <template <typename...> typename T>
struct template_wrapper
{
  template <typename... Args>
  using type = T<Args...>;
};

template <typename>
struct is_no_cv_template_wrapper : std::false_type
{
};

template <template <typename...> typename T>
struct is_no_cv_template_wrapper<template_wrapper<T>> : std::true_type
{
};

template <template <typename...> typename T, typename... BoundArgs>
struct is_no_cv_template_wrapper<bind_front<T, BoundArgs...>> : std::true_type
{
};

template <template <typename...> typename T, typename... BoundArgs>
struct is_no_cv_template_wrapper<bind_back<T, BoundArgs...>> : std::true_type
{
};

template <typename T>
constexpr bool is_no_cv_template_wrapper_v = is_no_cv_template_wrapper<T>::value;

template <typename T>
using is_template_wrapper = is_no_cv_template_wrapper<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_template_wrapper_v = is_template_wrapper<T>::value;

template <typename T>
concept wrapped_template = is_template_wrapper_v<T>;

template <wrapped_template T, typename... Args>
using invoke = T::template type<Args...>;

template <typename WrappedTemplate, typename... Args>
using invoke_t = invoke<WrappedTemplate, Args...>::type;

template <typename WrappedTemplate, typename... Args>
constexpr auto invoke_v = invoke<WrappedTemplate, Args...>::value;

} // namespace fmia::meta

namespace fmia::meta {

template <template <typename...> typename, typename>
struct is_predicate_tester;

// clang-format off
 
template <template <typename...> typename T, typename... Ts>
struct is_predicate_tester<T, type_list<Ts...>>
  : std::bool_constant<requires { { T<Ts...>::value } -> no_cvref_same_as<bool>; }>
{
};

// clang-format on

template <template <typename...> typename, typename>
struct is_predicate_impl;

template <template <typename...> typename T, std::size_t... Is>
struct is_predicate_impl<T, std::index_sequence<Is...>>
  : std::disjunction<is_predicate_tester<T, replicate_t<Is + 1, void>>...>
{
};

} // namespace fmia::meta

export namespace fmia::meta {

template <template <typename...> typename T, std::size_t ArityLimit = 5>
  requires (ArityLimit > 0)
struct is_predicate : is_predicate_impl<T, std::make_index_sequence<ArityLimit>>
{
};

template <template <typename...> typename T, std::size_t ArityLimit = 5>
constexpr bool is_predicate_v = is_predicate<T, ArityLimit>::value;

template <template <typename...> typename T>
concept predicate = is_predicate_v<T>;

template <typename T>
concept wrapped_predicate = wrapped_template<T> && predicate<T::template type>;

} // namespace fmia::meta

namespace fmia::meta {

template <typename... AnyTypeLists>
struct concat_impl
{
private:
  static constexpr bool is_indexed_ = is_indexed_type_list_v<AnyTypeLists...[0]>;

  // divide and conquer for >= 3 type lists for better time complexity:
  // 1. divide
  // `divide_helper_<BeginIdx, N / 2>` represents the left half
  // `divide_helper_<BeginIdx + N / 2, N - N / 2>` represents the right half
  // 2. merge
  // use the 2 lists specialization of `concat_impl` to merge
  //
  // placing this helper inside `concat_impl` improves compilation time significantly, since in this way we can avoid
  // the unnecessary copies of pack `Ts...` during the recursion
  template <std::size_t BeginIdx, std::size_t N>
  struct divide_helper_
    : concat_impl<
        typename divide_helper_<BeginIdx, N / 2>::type, typename divide_helper_<BeginIdx + N / 2, N - N / 2>::type
      >
  {
  };

  template <std::size_t BeginIdx>
  struct divide_helper_<BeginIdx, 1>
  {
    using type = AnyTypeLists...[BeginIdx];
  };

  template <std::size_t BeginIdx>
  struct divide_helper_<BeginIdx, 0>
  {
    using type = std::conditional_t<is_indexed_, empty_indexed_type_list, empty_type_list>;
  };

public:
  using type = divide_helper_<0, sizeof...(AnyTypeLists)>::type;
};

template <typename... Ts>
struct concat_impl<type_list<Ts...>>
{
  using type = type_list<Ts...>;
};

template <std::size_t... Is, typename... Ts>
struct concat_impl<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>>
{
  using type = indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>;
};

template <typename... Ts, typename... Us>
struct concat_impl<type_list<Ts...>, type_list<Us...>>
{
  using type = type_list<Ts..., Us...>;
};

template <std::size_t... Is, typename... Ts, std::size_t... Js, typename... Us>
struct concat_impl<
  indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>,
  indexed_type_list<std::index_sequence<Js...>, type_list<Us...>>
>
{
  using type = indexed_type_list<std::index_sequence<Is..., Js...>, type_list<Ts..., Us...>>;
};

template <typename Result, bool = is_indexed_type_list_v<Result>>
struct concat_get_result_helper;

template <typename Result>
struct concat_get_result_helper<Result, false>
{
  using type = Result;
};

template <typename Result>
  requires is_no_duplication_integer_sequence_v<typename Result::indices>
struct concat_get_result_helper<Result, true>
{
  using type = Result;
};

template <typename Result>
struct concat_get_result_helper<Result, true>
{
};

} // namespace fmia::meta

export namespace fmia::meta {

// get the concatenation of several type lists
//
// name after Haskell Data.List concat
//
// O(log n) time complexity for type_list, where n is the count of type lists to concatenate
// O(Nlog N) time complexity for indexed_type_list (because of the validation for indices), where N is the count of
// types over all given type lists
template <list_of_types... Ts>
using concat = concat_get_result_helper<typename concat_impl<Ts...>::type>;

template <typename... AnyTypeLists>
using concat_t = concat<AnyTypeLists...>::type;

// get a type list that contains types whose real indices are in the given `std::index_sequence`
//
// O(1) time complexity
template <typename, typename>
struct select_by_index_sequence;

template <std::size_t... Is, typename... Ts>
struct select_by_index_sequence<std::index_sequence<Is...>, type_list<Ts...>>
{
  using type = type_list<Ts...[Is]...>;
};

template <std::size_t... Is, std::size_t... Idxs, typename... Ts>
struct select_by_index_sequence<
  std::index_sequence<Is...>, indexed_type_list<std::index_sequence<Idxs...>, type_list<Ts...>>
>
{
  using type = indexed_type_list<std::index_sequence<Idxs...[Is]...>, type_list<Ts...[Is]...>>;
};

template <typename IndexSequence, typename AnyTypeList>
using select_by_index_sequence_t = select_by_index_sequence<IndexSequence, AnyTypeList>::type;

// get a type list that is the reverse of the given type list
//
// name after Haskell Data.List reverse
//
// O(1) time complexity
template <list_of_types T>
using reverse = select_by_index_sequence<make_reversed_index_sequence<length_v<T>>, T>;

template <typename AnyTypeList>
using reverse_t = reverse<AnyTypeList>::type;

// get a type list with the first type removed comparing to the given type list
//
// name after Haskell Data.List tail
//
// O(1) time complexity
template <nonempty_list_of_types T>
using tail = select_by_index_sequence<make_index_sequence_of_range<1, length_v<T> - 1>, T>;

template <typename AnyTypeList>
using tail_t = tail<AnyTypeList>::type;

// get a type list with the last type removed comparing to the given type list
//
// name after Haskell Data.List init
//
// O(1) time complexity
template <nonempty_list_of_types T>
using init = select_by_index_sequence<std::make_index_sequence<length_v<T> - 1>, T>;

template <typename AnyTypeList>
using init_t = init<AnyTypeList>::type;

// get a type list that contains the first N types of the given type list
//
// name after Haskell Data.List take
//
// O(1) time complexity
template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
using take = select_by_index_sequence<std::make_index_sequence<N>, T>;

template <std::size_t N, typename TypeList>
using take_t = take<N, TypeList>::type;

// same as take, but take from the end
//
// O(1) time complexity
template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
struct take_end : select_by_index_sequence<make_index_sequence_of_range<length_v<T> - N, length_v<T> - 1>, T>
{
};

template <typename T>
  requires is_type_list_v<T>
struct take_end<0, T>
{
  using type = empty_type_list;
};

template <typename T>
  requires is_indexed_type_list_v<T>
struct take_end<0, T>
{
  using type = empty_indexed_type_list;
};

template <std::size_t N, typename AnyTypeList>
using take_end_t = take_end<N, AnyTypeList>::type;

// get a type list with the first N types removed comparing to the given type list
//
// name after Haskell Data.List drop
//
// O(1) time complexity
template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
using drop = take_end<length_v<T> - N, T>;

template <std::size_t N, typename AnyTypeList>
using drop_t = drop<N, AnyTypeList>::type;

// same as drop, but drop from the end
//
// O(1) time complexity
template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
using drop_end = take<length_v<T> - N, T>;

template <std::size_t N, typename AnyTypeList>
using drop_end_t = drop_end<N, AnyTypeList>::type;

} // namespace fmia::meta

namespace fmia::meta {

template <template <typename> typename, typename, typename = empty_type_list>
struct take_while_impl;

template <template <typename> typename Pred, typename TypeListOfTaken>
struct take_while_impl<Pred, empty_type_list, TypeListOfTaken>
{
  using type = TypeListOfTaken;
};

template <template <typename> typename, typename, typename>
struct take_while_impl_lazy_evaluation_helper;

template <template <typename> typename Pred, typename T, typename... Ts, typename... Taken>
struct take_while_impl_lazy_evaluation_helper<Pred, type_list<T, Ts...>, type_list<Taken...>>
{
  // cannot use inheritance here, otherwise the evaluation is not lazy
  using type = take_while_impl<Pred, type_list<Ts...>, type_list<Taken..., T>>::type;
};

template <template <typename> typename Pred, typename T, typename... Ts, typename... Taken>
struct take_while_impl<Pred, type_list<T, Ts...>, type_list<Taken...>>
  : std::conditional_t<
      Pred<T>::value, take_while_impl_lazy_evaluation_helper<Pred, type_list<T, Ts...>, type_list<Taken...>>,
      std::type_identity<type_list<Taken...>>
    >
{
};

} // namespace fmia::meta

export namespace fmia::meta {

// get the longest prefix type list whose types all satisfy a given predicate
//
// name after Haskell Data.List takeWhile
//
// O(k) time complexity, where k is the length of the longest prefix
template <template <typename> typename Pred, list_of_types T>
  requires predicate<Pred>
using take_while = take_while_impl<Pred, T>;

template <template <typename> typename Pred, typename TypeList>
using take_while_t = take_while<Pred, TypeList>::type;

// get the longest suffix type list whose types all satisfy a given predicate
//
// O(k) time complexity, where k is the length of the longest suffix
template <template <typename> typename Pred, list_of_types T>
  requires predicate<Pred>
struct take_while_end : reverse<take_while_t<Pred, reverse_t<T>>>
{
};

template <template <typename> typename Pred, typename TypeList>
using take_while_end_t = take_while_end<Pred, TypeList>::type;

} // namespace fmia::meta

namespace fmia::meta {

template <template <typename> typename, typename>
struct drop_while_impl;

template <template <typename> typename Pred>
struct drop_while_impl<Pred, empty_type_list>
{
  using type = empty_type_list;
};

template <template <typename> typename, typename>
struct drop_while_impl_lazy_evaluation_helper;

template <template <typename> typename Pred, typename T, typename... Ts>
struct drop_while_impl_lazy_evaluation_helper<Pred, type_list<T, Ts...>>
{
  // cannot use inheritance here, otherwise the evaluation is not lazy
  using type = drop_while_impl<Pred, type_list<Ts...>>::type;
};

template <template <typename> typename Pred, typename T, typename... Ts>
struct drop_while_impl<Pred, type_list<T, Ts...>>
  : std::conditional_t<
      Pred<T>::value, drop_while_impl_lazy_evaluation_helper<Pred, type_list<T, Ts...>>,
      std::type_identity<type_list<T, Ts...>>
    >
{
};

} // namespace fmia::meta

export namespace fmia::meta {

// get a type list with a longest prefix type list removed, whose types all satisfy a given predicate
//
// name after Haskell Data.List dropWhile
//
// O(k) time complexity, where k is the longest dropped prefix
template <template <typename> typename Pred, list_of_types T>
  requires predicate<Pred>
using drop_while = drop_while_impl<Pred, T>;

template <template <typename> typename Pred, typename TypeList>
using drop_while_t = drop_while<Pred, TypeList>::type;

// get a type list with a longest suffix type list removed, whose types all satisfy a given predicate
//
// name after Haskell Data.List dropWhileEnd
//
// O(k) time complexity, where k is the length of the longest dropped suffix
template <template <typename> typename Pred, list_of_types T>
  requires predicate<Pred>
struct drop_while_end : reverse<drop_while_t<Pred, reverse_t<T>>>
{
};

template <template <typename> typename Pred, typename TypeList>
using drop_while_end_t = drop_while_end<Pred, TypeList>::type;

// get a type list that contains all the types that satisfy a given predicate
//
// name after Haskell Data.List filter
//
// O(log n) time complexity, limited by `concat`, where n is the length of the given type list
template <template <typename> typename Pred, typename>
  requires predicate<Pred>
struct filter;

template <template <typename> typename Pred>
struct filter<Pred, empty_type_list>
{
  using type = empty_type_list;
};

template <template <typename> typename Pred, typename... Ts>
struct filter<Pred, type_list<Ts...>> : concat<std::conditional_t<Pred<Ts>::value, type_list<Ts>, empty_type_list>...>
{
};

template <template <typename> typename Pred, typename TypeList>
using filter_t = filter<Pred, TypeList>::type;

// get a type list that only contains the given type T comparing to the given type list
//
// O(log n) time complexity, where n is the length of the given type list
template <typename T, list_of_types U>
using keep = filter<bind_front<std::is_same, T>::template type, U>;

template <typename T, typename TypeList>
using keep_t = keep<T, TypeList>::type;

// get a type list that does not contain the given type T comparing to the given type list
//
// O(log n) time complexity, where n is the length of the given type list
template <typename T, list_of_types U>
using remove = filter<bind_front<not_same, T>::template type, U>;

template <typename T, typename TypeList>
using remove_t = remove<T, TypeList>::type;

// apply the given template to every type of the given type list
//
// O(1) time complexity
template <template <typename> typename, typename>
struct transform;

template <template <typename> typename T, typename... Us>
struct transform<T, type_list<Us...>>
{
  using type = type_list<typename T<Us>::type...>;
};

template <template <typename> typename T, typename TypeList>
using transform_t = transform<T, TypeList>::type;

// test if any of the types of the given type list satisfy the given predicate
//
// name after Haskell Data.List any
//
// O(k) time complexity, where k is the length of the longest prefix whose types do not satisfy the given predicate
template <template <typename> typename T, typename>
  requires predicate<T>
struct any;

template <template <typename> typename T, typename... Us>
struct any<T, type_list<Us...>> : std::disjunction<T<Us>...>
{
};

template <template <typename> typename T, typename TypeList>
constexpr bool any_v = any<T, TypeList>::value;

// test if none of the types of the given type list satisfy the given predicate
template <template <typename> typename T, list_of_types U>
using none = std::negation<any<T, U>>;

template <template <typename> typename T, typename TypeList>
constexpr bool none_v = none<T, TypeList>::value;

// test if all of the types of the given type list satisfy the given predicate
//
// name after Haskell Data.List all
//
// O(k) time complexity, where k is the length of the longest prefix whose types satisfy the given predicate
template <template <typename> typename T, typename>
  requires predicate<T>
struct all;

template <template <typename> typename T>
struct all<T, empty_type_list> : std::false_type
{
};

template <template <typename> typename T, typename... Us>
struct all<T, type_list<Us...>> : std::conjunction<T<Us>...>
{
};

template <template <typename> typename T, typename TypeList>
constexpr bool all_v = all<T, TypeList>::value;

template <typename, typename>
struct has_any;

template <typename T>
struct has_any<T, empty_type_list> : std::false_type
{
};

template <typename T>
struct has_any<T, empty_indexed_type_list> : std::false_type
{
};

template <typename T, typename... Us>
struct has_any<T, type_list<Us...>> : is_any_of<T, Us...>
{
};

template <typename T, std::size_t... Is, typename... Us>
struct has_any<T, indexed_type_list<std::index_sequence<Is...>, type_list<Us...>>> : is_any_of<T, Us...>
{
};

template <typename T, typename AnyTypeList>
constexpr bool has_any_v = has_any<T, AnyTypeList>::value;

template <typename T, list_of_types U>
using has_none = std::negation<has_any<T, U>>;

template <typename T, typename U>
constexpr bool has_none_v = has_none<T, U>::value;

} // namespace fmia::meta
