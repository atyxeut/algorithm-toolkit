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

export module fmia.data_structure.graph.storage;

import std;

import fmia.math;

export namespace fmia::graph {

template <typename>
struct weight;

template <>
struct weight<void>
{
};

template <meta::arithmetic T>
struct weight<T>
{
  static constexpr auto infinity = std::numeric_limits<T>::max();
  static constexpr auto negative_infinity = std::numeric_limits<T>::min();

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

}; // namespace fmia::graph

export namespace fmia::meta {

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
concept graph = unweighted_graph<T> || weighted_graph<T>;

} // namespace fmia::meta

export namespace fmia::meta {

template <typename T>
struct is_no_cv_edge_list : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_edge_list_v = is_no_cv_edge_list<T>::value;

template <typename T>
concept edge_list = is_no_cv_edge_list_v<std::remove_cvref_t<T>>;

} // namespace fmia::meta

namespace fmia::graph::detail {

template <typename Vertex, typename Weight>
class basic_edge_list_base
{
public:
  using vertex_type = Vertex;
  using weight_type = Weight;

protected:
  std::vector<edge<vertex_type, weight_type>> edges_;

public:
  [[nodiscard]] constexpr bool empty() const noexcept { return edges_.empty(); }

  [[nodiscard]] constexpr auto edge_size() const noexcept { return edges_.size(); }

  [[nodiscard]] constexpr auto begin() noexcept { return edges_.begin(); }
  [[nodiscard]] constexpr auto begin() const noexcept { return edges_.begin(); }

  [[nodiscard]] constexpr auto end() noexcept { return edges_.end(); }
  [[nodiscard]] constexpr auto end() const noexcept { return edges_.end(); }

  [[nodiscard]] constexpr auto& operator [](usize idx) { return edges_[idx]; }
  [[nodiscard]] constexpr auto& operator [](usize idx) const { return edges_[idx]; }

  constexpr void reserve(usize capacity) { edges_.reserve(capacity); }
};

template <typename Vertex, typename Weight>
class edge_list_base : public basic_edge_list_base<Vertex, Weight>
{
private:
  using base_ = basic_edge_list_base<Vertex, Weight>;

protected:
  std::unordered_map<typename base_::vertex_type_, bool> exist_;

public:
  [[nodiscard]] constexpr auto vertex_size() const noexcept { return static_cast<Vertex>(exist_.size()); }
};

} // namespace fmia::graph::detail

export namespace fmia::graph {

template <std::integral Vertex>
struct basic_unweighted_edge_list : public detail::basic_edge_list_base<Vertex, void>
{
  constexpr auto& add_edge(Vertex u, Vertex v) { return this->edges_.emplace_back(u, v); }
};

template <std::integral Vertex, meta::arithmetic Weight>
struct basic_weighted_edge_list : public detail::basic_edge_list_base<Vertex, Weight>
{
  constexpr auto& add_edge(Vertex u, Vertex v, const Weight& w) { return this->edges_.emplace_back(u, v, w); }
  constexpr auto& add_edge(Vertex u, Vertex v, Weight&& w) { return this->edges_.emplace_back(u, v, std::move(w)); }
};

} // namespace fmia::graph

export namespace fmia::meta {

template <typename Vertex>
struct is_no_cv_edge_list<graph::basic_unweighted_edge_list<Vertex>> : std::true_type
{
};

template <typename Vertex, typename Weight>
struct is_no_cv_edge_list<graph::basic_weighted_edge_list<Vertex, Weight>> : std::true_type
{
};

} // namespace fmia::meta

export namespace fmia::graph {

template <std::integral Vertex>
class unweighted_edge_list : public detail::edge_list_base<Vertex, void>
{
public:
  constexpr auto& add_edge(Vertex u, Vertex v)
  {
    this->exist_[u] = this->exist_[v] = true;
    return this->edges_.emplace_back(u, v);
  }
};

template <std::integral Vertex, meta::arithmetic Weight>
class weighted_edge_list : public detail::edge_list_base<Vertex, Weight>
{
public:
  constexpr auto& add_edge(Vertex u, Vertex v, const Weight& w)
  {
    this->exist_[u] = this->exist_[v] = true;
    return this->edges_.emplace_back(u, v, w);
  }

  constexpr auto& add_edge(Vertex u, Vertex v, Weight&& w)
  {
    this->exist_[u] = this->exist_[v] = true;
    return this->edges_.emplace_back(u, v, std::move(w));
  }
};

} // namespace fmia::graph

export namespace fmia::meta {

template <typename Vertex>
struct is_no_cv_edge_list<graph::unweighted_edge_list<Vertex>> : std::true_type
{
};

template <typename Vertex>
struct is_no_cv_unweighted_graph<graph::unweighted_edge_list<Vertex>> : std::true_type
{
};

template <typename Vertex, typename Weight>
struct is_no_cv_edge_list<graph::weighted_edge_list<Vertex, Weight>> : std::true_type
{
};

template <typename Vertex, typename Weight>
struct is_no_cv_weighted_graph<graph::weighted_edge_list<Vertex, Weight>> : std::true_type
{
};

} // namespace fmia::meta
