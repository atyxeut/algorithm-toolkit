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

} // namespace detail

// extract the cv-qualifiers of From and apply them to To
export template <typename From, typename To>
using claim_cv = detail::claim_cv_selector<From, To>;

export template <typename From, typename To>
using claim_cv_t = claim_cv<From, To>::type;

export template <typename T, typename U>
struct not_same : std::negation<std::is_same<T, U>>
{
};

export template <typename T, typename U>
constexpr bool not_same_v = not_same<T, U>::value;

export template <typename T, typename U>
concept no_cvref_same_as = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

export template <typename T, typename U>
concept no_cvref_not_same_as = !no_cvref_same_as<T, U>;

export template <std::size_t N>
using index_constant = std::integral_constant<std::size_t, N>;

namespace detail {

template <typename>
struct make_reversed_integer_sequence_impl;

template <typename Int, Int... Is>
struct make_reversed_integer_sequence_impl<std::integer_sequence<Int, Is...>>
{
  using type = std::integer_sequence<Int, (sizeof...(Is) - 1 - Is)...>;
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

template <typename Int, Int, typename>
struct make_integer_sequence_of_range_impl;

template <typename Int, Int Begin, Int... Is>
struct make_integer_sequence_of_range_impl<Int, Begin, std::integer_sequence<Int, Is...>>
{
  using type = std::integer_sequence<Int, (Begin + Is)...>;
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

template <typename Int, Int, typename>
struct make_reversed_integer_sequence_of_range_impl;

template <typename Int, Int End, Int... Is>
struct make_reversed_integer_sequence_of_range_impl<Int, End, std::integer_sequence<Int, Is...>>
{
  using type = std::integer_sequence<Int, (End - Is)...>;
};

} // namespace detail

// generate a sequence of integers of type T in [Begin, End] in a reversed order
// O(1) time complexity, assume `std::make_integer_sequence` will be optimized by compiler intrinsics, i.e. not a naive recursive implementation
export template <std::integral T, T Begin, T End>
  requires (Begin <= End)
using make_reversed_integer_sequence_of_range = detail::make_reversed_integer_sequence_of_range_impl<T, End, std::make_integer_sequence<T, End - Begin + 1>>::type;

export template <std::size_t Begin, std::size_t End>
using make_reversed_index_sequence_of_range = make_reversed_integer_sequence_of_range<std::size_t, Begin, End>;

// add an offset to all the integers in the given `std::integer_sequence`
// O(1) time complexity
export template <std::integral T, T, typename>
struct shift_integer_sequence;

template <typename Int, Int Offset, Int... Is>
struct shift_integer_sequence<Int, Offset, std::integer_sequence<Int, Is...>>
{
  using type = std::integer_sequence<Int, (Is + Offset)...>;
};

export template <typename Int, Int Offset, typename IntegerSequence>
using shift_integer_sequence_t = shift_integer_sequence<Int, Offset, IntegerSequence>::type;

// add an offset to all indices in the given `std::index_sequence`
// O(1) time complexity
export template <std::size_t Offset, typename IndexSequence>
using shift_index_sequence = shift_integer_sequence<std::size_t, Offset, IndexSequence>;

export template <std::size_t Offset, typename IndexSequence>
using shift_index_sequence_t = shift_index_sequence<Offset, IndexSequence>::type;

export template <typename T, typename... Us>
struct is_any_of : std::disjunction<is_any_of<T, Us>...>
{
};

template <typename T, typename U>
struct is_any_of<T, U> : std::bool_constant<std::same_as<T, U>>
{
};

export template <typename T, typename... Us>
constexpr bool is_any_of_v = is_any_of<T, Us...>::value;

export template <typename T, typename... Us>
using is_none_of = std::negation<is_any_of<T, Us...>>;

export template <typename T, typename... Us>
constexpr bool is_none_of_v = is_none_of<T, Us...>::value;

export template <typename...>
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

export template <typename... Ts>
constexpr bool all_the_same_v = all_the_same<Ts...>::value;

export template <typename... Ts>
struct type_list
{
};

export using empty_type_list = type_list<>;

export template <typename>
struct is_no_cv_type_list : std::false_type
{
};

template <typename... Ts>
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
struct is_no_cv_empty_type_list : std::false_type
{
};

template <>
struct is_no_cv_empty_type_list<empty_type_list> : std::true_type
{
};

export template <typename T>
constexpr bool is_no_cv_empty_type_list_v = is_no_cv_empty_type_list<T>::value;

export template <typename T>
using is_empty_type_list = is_no_cv_empty_type_list<std::remove_cv_t<T>>;

export template <typename T>
constexpr bool is_empty_type_list_v = is_empty_type_list<T>::value;

export template <std::size_t I, typename T>
struct indexed_type
{
  using type = T;
  static constexpr auto vaule = I;
};

export template <std::size_t I, typename T>
using indexed_type_t = indexed_type<I, T>::type;

export template <std::size_t I, typename T>
constexpr auto indexed_type_v = indexed_type<I, T>::value;

export template <typename>
struct unwrap_indexed_type;

template <std::size_t I, typename T>
struct unwrap_indexed_type<indexed_type<I, T>> : indexed_type<I, T>
{
};

export template <typename IndexedType>
using unwrap_indexed_type_t = unwrap_indexed_type<IndexedType>::type;

export template <typename IndexedType>
constexpr auto unwrap_indexed_type_v = unwrap_indexed_type<IndexedType>::value;

export template <typename, typename>
struct indexed_type_list;

template <std::size_t... Is, typename... Ts>
struct indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>> : indexed_type<Is, Ts>...
{
};

export using empty_indexed_type_list = indexed_type_list<std::index_sequence<>, type_list<>>;

export template <typename>
struct is_no_cv_indexed_type_list : std::false_type
{
};

template <std::size_t... Is, typename... Ts>
struct is_no_cv_indexed_type_list<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>> : std::true_type
{
};

export template <typename T>
constexpr bool is_no_cv_indexed_type_list_v = is_no_cv_indexed_type_list<T>::value;

export template <typename T>
using is_indexed_type_list = is_no_cv_indexed_type_list<std::remove_cv_t<T>>;

export template <typename T>
constexpr bool is_indexed_type_list_v = is_indexed_type_list<T>::value;

export template <typename T>
struct is_no_cv_empty_indexed_type_list : std::false_type
{
};

template <>
struct is_no_cv_empty_indexed_type_list<empty_indexed_type_list> : std::true_type
{
};

export template <typename T>
constexpr bool is_no_cv_empty_indexed_type_list_v = is_no_cv_empty_indexed_type_list<T>::value;

export template <typename T>
using is_empty_indexed_type_list = is_no_cv_empty_indexed_type_list<std::remove_cv_t<T>>;

export template <typename T>
constexpr bool is_empty_indexed_type_list_v = is_empty_indexed_type_list<T>::value;

export template <typename T>
concept list_of_types = is_type_list_v<T> || is_indexed_type_list_v<T>;

export template <typename T>
concept empty_list_of_types = is_empty_type_list_v<T> || is_empty_indexed_type_list_v<T>;

export template <typename T>
concept nonempty_list_of_types = list_of_types<T> && !empty_list_of_types<T>;

// get the length of a type list
// O(1) time complexity
// name after Haskell Data.List length
export template <typename>
struct length;

template <typename... Ts>
struct length<type_list<Ts...>> : index_constant<sizeof...(Ts)>
{
};

template <std::size_t... Is, typename... Ts>
struct length<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>> : index_constant<sizeof...(Ts)>
{
};

export template <typename AnyTypeList>
constexpr std::size_t length_v = length<AnyTypeList>::value;

// get the nth type of a type list, index starts at 0
// O(1) time complexity, assume C++26 pack indexing has O(1) time complexity
export template <std::size_t Idx, typename AnyTypeList>
  requires (Idx < length_v<AnyTypeList>)
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

export template <std::size_t Idx, typename AnyTypeList>
using nth_t = nth<Idx, AnyTypeList>::type;

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

namespace detail {

template <std::size_t I, typename T>
indexed_type<I, T> lookup_indexed_type_helper(indexed_type<I, T>);

} // namespace detail

// get the corresponding indexed type with a given index
// O(1) time complexity
// name after Haskell Data.List lookup
export template <std::size_t I, typename IndexedTypeList>
  requires is_indexed_type_list_v<IndexedTypeList>
struct lookup
{
  using type = decltype(detail::lookup_indexed_type_helper<I>(std::declval<IndexedTypeList>()));
};

export template <std::size_t I, typename IndexedTypeList>
using lookup_t = lookup<I, IndexedTypeList>::type;

// get a type list that has one element added to the beginning comparing to the given type list
// O(1) time complexity
// name after Haskell Data.List : operator (1 : [1, 2] --> [1, 1, 2])
export template <typename, typename>
struct cons;

template <typename T, typename... Ts>
struct cons<T, type_list<Ts...>>
{
  using type = type_list<T, Ts...>;
};

export template <typename T, typename AnyTypeList>
using cons_t = cons<T, AnyTypeList>::type;

// get a type list that has one element added to the end comparing to the given type list
// O(1) time complexity
export template <typename, typename>
struct snoc;

template <typename T, typename... Ts>
struct snoc<T, type_list<Ts...>>
{
  using type = type_list<Ts..., T>;
};

export template <typename T, typename AnyTypeList>
using snoc_t = snoc<T, AnyTypeList>::type;

namespace detail {

template <typename, typename>
struct replicate_impl;

template <typename T, std::size_t... Is>
struct replicate_impl<T, std::index_sequence<Is...>>
{
  using type = type_list<std::enable_if_t<(Is >= 0), T>...>;
};

} // namespace detail

// get a type list that contains N identical types
// O(1) time complexity
// name after Haskell Data.List replicate
export template <std::size_t N, typename T>
using replicate = detail::replicate_impl<T, std::make_index_sequence<N>>;

export template <std::size_t N, typename T>
using replicate_t = replicate<N, T>::type;

// get the concatenation of several type lists
// O(log n) time complexity, where n is the count of type lists to concatenate
// name after Haskell Data.List concat
export template <list_of_types... Ts>
struct concat
{
private:
  // divide and conquer for >= 3 type lists for better time complexity:
  // 1. divide
  // `divide_helper<BeginIdx, N / 2>` represents the left half
  // `divide_helper<BeginIdx + N / 2, N - N / 2>` represents the right half
  // 2. merge
  // use the 2 lists specialization of `concat` to merge
  //
  // note: placing this helper inside improves compilation time significantly, since in this way we can avoid the unnecessary copies of pack `Ts...` during the recursion
  template <std::size_t BeginIdx, std::size_t N>
  struct divide_helper : concat<typename divide_helper<BeginIdx, N / 2>::type, typename divide_helper<BeginIdx + N / 2, N - N / 2>::type>
  {
  };

  template <std::size_t BeginIdx>
  struct divide_helper<BeginIdx, 1>
  {
    using type = Ts...[BeginIdx];
  };

  template <std::size_t BeginIdx>
  struct divide_helper<BeginIdx, 0>
  {
    using type = empty_type_list;
  };

public:
  using type = divide_helper<0, sizeof...(Ts)>::type;
};

template <typename... Ts>
struct concat<type_list<Ts...>>
{
  using type = type_list<Ts...>;
};

template <typename... Ts, typename... Us>
struct concat<type_list<Ts...>, type_list<Us...>>
{
  using type = type_list<Ts..., Us...>;
};

export template <typename... TypeLists>
using concat_t = concat<TypeLists...>::type;

// get a type list that contains types whose indices are in the given `std::index_sequence`
// O(1) time complexity
export template <typename, list_of_types>
struct select_by_index_sequence;

template <std::size_t... Is, typename... Ts>
struct select_by_index_sequence<std::index_sequence<Is...>, type_list<Ts...>>
{
  using type = type_list<Ts...[Is]...>;
};

export template <typename IndexSequence, typename TypeList>
using select_by_index_sequence_t = select_by_index_sequence<IndexSequence, TypeList>::type;

// get a type list that is the reverse of the given type list
// O(1) time complexity
// name after Haskell Data.List reverse
export template <list_of_types T>
using reverse = select_by_index_sequence<make_reversed_index_sequence<length_v<T>>, T>;

export template <typename TypeList>
using reverse_t = reverse<TypeList>::type;

// get a type list with the first type removed comparing to the given type list
// O(1) time complexity
// name after Haskell Data.List tail
export template <nonempty_list_of_types>
struct tail;

template <typename T, typename... Ts>
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

export template <typename TypeList>
using init_t = init<TypeList>::type;

// get a type list that contains the first N types of the given type list
// O(1) time complexity
// name after Haskell Data.List take
export template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
using take = select_by_index_sequence<std::make_index_sequence<N>, T>;

export template <std::size_t N, typename TypeList>
using take_t = take<N, TypeList>::type;

// same as take, but take from the end
// O(1) time complexity
export template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
struct take_end : select_by_index_sequence<make_index_sequence_of_range<length_v<T> - N, length_v<T> - 1>, T>
{
};

template <typename TypeList>
struct take_end<0, TypeList>
{
  using type = empty_type_list;
};

export template <std::size_t N, typename TypeList>
using take_end_t = take_end<N, TypeList>::type;

// get a type list with the first N types removed comparing to the given type list
// O(1) time complexity
// name after Haskell Data.List drop
export template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
using drop = take_end<length_v<T> - N, T>;

export template <std::size_t N, typename TypeList>
using drop_t = drop<N, TypeList>::type;

// same as drop, but drop from the end
// O(1) time complexity
export template <std::size_t N, list_of_types T>
  requires (N <= length_v<T>)
using drop_end = take<length_v<T> - N, T>;

export template <std::size_t N, typename TypeList>
using drop_end_t = drop_end<N, TypeList>::type;

namespace detail {

template <template <typename...> typename, typename>
struct is_predicate_tester;

// clang-format off
template <template <typename...> typename T, typename... Ts>
struct is_predicate_tester<T, type_list<Ts...>> : std::bool_constant<requires { { T<Ts...>::value } -> no_cvref_same_as<bool>; }>
{
};
// clang-format on

template <template <typename...> typename, typename>
struct is_predicate_impl;

template <template <typename...> typename T, std::size_t... Is>
struct is_predicate_impl<T, std::index_sequence<Is...>> : std::disjunction<is_predicate_tester<T, replicate_t<Is + 1, void>>...>
{
};

} // namespace detail

export template <template <typename...> typename T, std::size_t ArityLimit = 5>
  requires (ArityLimit > 0)
struct is_predicate : detail::is_predicate_impl<T, std::make_index_sequence<ArityLimit>>
{
};

export template <template <typename...> typename T, std::size_t ArityLimit = 5>
constexpr bool is_predicate_v = is_predicate<T, ArityLimit>::value;

export template <template <typename...> typename T>
concept predicate = is_predicate_v<T>;

export template <template <typename...> typename T>
struct template_wrapper
{
  template <typename... Args>
  using type = T<Args...>;
};

export template <template <typename...> typename T, typename... BoundArgs>
struct bind_front
{
  template <typename... CallArgs>
  using type = T<BoundArgs..., CallArgs...>;
};

export template <template <typename...> typename T, typename... BoundArgs>
struct bind_back
{
  template <typename... CallArgs>
  using type = T<CallArgs..., BoundArgs...>;
};

export template <typename>
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

export template <wrapped_template T, typename... Args>
using invoke = T::template type<Args...>;

export template <typename WrappedTemplate, typename... Args>
using invoke_t = invoke<WrappedTemplate, Args...>::type;

export template <typename WrappedTemplate, typename... Args>
constexpr auto invoke_v = invoke<WrappedTemplate, Args...>::value;

namespace detail {

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
struct take_while_impl<Pred, type_list<T, Ts...>, type_list<Taken...>> : std::conditional_t<Pred<T>::value, take_while_impl_lazy_evaluation_helper<Pred, type_list<T, Ts...>, type_list<Taken...>>, std::type_identity<type_list<Taken...>>>
{
};

} // namespace detail

// get the longest prefix type list whose types all satisfy a given predicate
// O(k) time complexity, where k is the length of the longest prefix
// name after Haskell Data.List takeWhile
export template <template <typename> typename Pred, list_of_types T>
  requires predicate<Pred>
using take_while = detail::take_while_impl<Pred, T>;

export template <template <typename> typename Pred, typename TypeList>
using take_while_t = take_while<Pred, TypeList>::type;

// get the longest suffix type list whose types all satisfy a given predicate
// O(k) time complexity, where k is the length of the longest suffix
export template <template <typename> typename Pred, list_of_types T>
  requires predicate<Pred>
struct take_while_end : reverse<take_while_t<Pred, reverse_t<T>>>
{
};

export template <template <typename> typename Pred, typename TypeList>
using take_while_end_t = take_while_end<Pred, TypeList>::type;

namespace detail {

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
struct drop_while_impl<Pred, type_list<T, Ts...>> : std::conditional_t<Pred<T>::value, drop_while_impl_lazy_evaluation_helper<Pred, type_list<T, Ts...>>, std::type_identity<type_list<T, Ts...>>>
{
};

} // namespace detail

// get a type list with a longest prefix type list removed, whose types all satisfy a given predicate
// O(k) time complexity, where k is the longest dropped prefix
// name after Haskell Data.List dropWhile
export template <template <typename> typename Pred, list_of_types T>
  requires predicate<Pred>
using drop_while = detail::drop_while_impl<Pred, T>;

export template <template <typename> typename Pred, typename TypeList>
using drop_while_t = drop_while<Pred, TypeList>::type;

// get a type list with a longest suffix type list removed, whose types all satisfy a given predicate
// O(k) time complexity, where k is the length of the longest dropped suffix
// name after Haskell Data.List dropWhileEnd
export template <template <typename> typename Pred, list_of_types T>
  requires predicate<Pred>
struct drop_while_end : reverse<drop_while_t<Pred, reverse_t<T>>>
{
};

export template <template <typename> typename Pred, typename TypeList>
using drop_while_end_t = drop_while_end<Pred, TypeList>::type;

// get a type list that contains all the types that satisfy a given predicate
// O(log n) time complexity, limited by `concat`, where n is the length of the given type list
// name after Haskell Data.List filter
export template <template <typename> typename Pred, typename>
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

export template <template <typename> typename Pred, typename TypeList>
using filter_t = filter<Pred, TypeList>::type;

// get a type list that only contains the given type T comparing to the given type list
// O(log n) time complexity, where n is the length of the given type list
export template <typename T, list_of_types U>
using keep = filter<bind_front<std::is_same, T>::template type, U>;

export template <typename T, typename TypeList>
using keep_t = keep<T, TypeList>::type;

// get a type list that does not contain the given type T comparing to the given type list
// O(log n) time complexity, where n is the length of the given type list
export template <typename T, list_of_types U>
using remove = filter<bind_front<not_same, T>::template type, U>;

export template <typename T, typename TypeList>
using remove_t = remove<T, TypeList>::type;

// apply the given template to every type of the given type list
// O(1) time complexity
export template <template <typename> typename, typename>
struct transform;

template <template <typename> typename T, typename... Us>
struct transform<T, type_list<Us...>>
{
  using type = type_list<typename T<Us>::type...>;
};

export template <template <typename> typename T, typename TypeList>
using transform_t = transform<T, TypeList>::type;

// test if any of the types of the given type list satisfy the given predicate
// O(k) time complexity, where k is the length of the longest prefix whose types do not satisfy the given predicate
// name after Haskell Data.List any
export template <template <typename> typename T, typename>
  requires predicate<T>
struct any;

template <template <typename> typename T, typename... Us>
struct any<T, type_list<Us...>> : std::disjunction<T<Us>...>
{
};

export template <template <typename> typename T, typename TypeList>
constexpr bool any_v = any<T, TypeList>::value;

// test if none of the types of the given type list satisfy the given predicate
export template <template <typename> typename T, list_of_types U>
using none = std::negation<any<T, U>>;

export template <template <typename> typename T, typename TypeList>
constexpr bool none_v = none<T, TypeList>::value;

// test if all of the types of the given type list satisfy the given predicate
// O(k) time complexity, where k is the length of the longest prefix whose types satisfy the given predicate
// name after Haskell Data.List all
export template <template <typename> typename T, typename>
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

export template <template <typename> typename T, typename TypeList>
constexpr bool all_v = all<T, TypeList>::value;

export template <typename, typename>
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

export template <typename T, typename AnyTypeList>
constexpr bool has_any_v = has_any<T, AnyTypeList>::value;

export template <typename T, list_of_types U>
using has_none = std::negation<has_any<T, U>>;

export template <typename T, typename U>
constexpr bool has_none_v = has_none<T, U>::value;

} // namespace aatk::meta
