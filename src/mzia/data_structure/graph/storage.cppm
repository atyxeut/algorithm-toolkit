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

export module mzia.data_structure.graph.storage;

import std;

import mzia.math;

export namespace mzia::graph {

template <typename>
struct weight;

template <>
struct weight<void>
{
};

template <meta::arithmetic T>
struct weight<T>
{
  T w;
};

template <std::integral Vertex, typename Weight = void>
struct edge_from : weight<Weight>
{
  Vertex u;
};

template <std::integral Vertex, typename Weight = void>
struct edge_to : weight<Weight>
{
  Vertex v;
};

template <std::integral Vertex, typename Weight = void>
struct edge : weight<Weight>
{
  Vertex u;
  Vertex v;
};

}; // namespace mzia::graph

export namespace mzia::meta {

template <typename T>
struct is_no_cv_unweighted_graph : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_unweighted_graph_v = is_no_cv_unweighted_graph<T>::value;

template <typename T>
concept unweighted_graph = is_no_cv_unweighted_graph_v<std::remove_cvref_t<T>>;

template <typename T>
struct is_no_cv_weighted_graph : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_weighted_graph_v = is_no_cv_weighted_graph<T>::value;

template <typename T>
concept weighted_graph = is_no_cv_weighted_graph_v<std::remove_cvref_t<T>>;

template <typename T>
struct is_no_cv_graph : std::disjunction<is_no_cv_unweighted_graph<T>, is_no_cv_weighted_graph<T>>
{
};

template <typename T>
constexpr bool is_no_cv_graph_v = is_no_cv_graph<T>::value;

template <typename T>
concept graph = is_no_cv_graph_v<std::remove_cvref_t<T>>;

} // namespace mzia::meta

namespace mzia::graph {

namespace detail {

template <typename Vertex, typename Weight>
class edge_list_base
{
protected:
  std::vector<edge<Vertex, Weight>> edges_;

public:
  [[nodiscard]] constexpr auto size() const noexcept { return edges_.size(); }

  [[nodiscard]] constexpr auto begin() noexcept { return edges_.begin(); }
  [[nodiscard]] constexpr auto begin() const noexcept { return edges_.begin(); }

  [[nodiscard]] constexpr auto end() noexcept { return edges_.end(); }
  [[nodiscard]] constexpr auto end() const noexcept { return edges_.end(); }

  [[nodiscard]] auto& operator [](usize idx) { return edges_[idx]; }
  [[nodiscard]] constexpr const auto& operator [](usize idx) const { return edges_[idx]; }

  constexpr void reserve(usize capacity) { edges_.reserve(capacity); }
};

} // namespace detail

export template <std::integral Vertex>
class unweighted_edge_list : public detail::edge_list_base<Vertex, void>
{
public:
  constexpr auto& add_edge(Vertex u, Vertex v) { return this->edges_.emplace_back(u, v); }
};

export template <std::integral Vertex, meta::arithmetic Weight>
class weighted_edge_list : public detail::edge_list_base<Vertex, Weight>
{
public:
  constexpr auto& add_edge(Vertex u, Vertex v, const Weight& w) { return this->edges_.emplace_back(u, v, w); }
  constexpr auto& add_edge(Vertex u, Vertex v, Weight&& w) { return this->edges_.emplace_back(u, v, std::move(w)); }
};

} // namespace mzia::graph

export namespace mzia::meta {

template <typename Vertex>
struct is_no_cv_unweighted_graph<graph::unweighted_edge_list<Vertex>> : std::true_type
{
};

template <typename Vertex, typename Weight>
struct is_no_cv_weighted_graph<graph::weighted_edge_list<Vertex, Weight>> : std::true_type
{
};

} // namespace mzia::meta
