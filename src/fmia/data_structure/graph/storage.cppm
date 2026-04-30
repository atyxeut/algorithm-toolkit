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

enum class graph_tag : u8 { undirected, directed };

} // namespace fmia::graph

namespace fmia::graph {

template <typename Vertex>
struct edge_from_base
{
  Vertex u;
};

template <typename Vertex>
struct edge_to_base
{
  Vertex v;
};

template <typename Vertex>
struct edge_base : edge_from_base<Vertex>, edge_to_base<Vertex>
{
};

template <typename Degree>
struct edge_id
{
  Degree id;
};

} // namespace fmia::graph

export namespace fmia::graph {

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree>
struct unweighted_edge_from : edge_from_base<Vertex>, edge_id<Degree>
{
};

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree>
struct unweighted_edge_to : edge_to_base<Vertex>, edge_id<Degree>
{
};

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree>
struct unweighted_edge : edge_base<Vertex>, edge_id<Degree>
{
};

} // namespace fmia::graph

namespace fmia::graph {

template <typename>
struct weight;

template <>
struct weight<void>
{
  static constexpr int w = 1;
};

template <meta::arithmetic T>
struct weight<T>
{
  static constexpr auto infinity = std::numeric_limits<T>::max();
  static constexpr auto negative_infinity = std::numeric_limits<T>::min();

  T w;
};

} // namespace fmia::graph

export namespace fmia::graph {

template <std::signed_integral Vertex, typename Weight>
struct weighted_edge_from : edge_from_base<Vertex>, weight<Weight>
{
};

template <std::signed_integral Vertex, typename Weight>
struct weighted_edge_to : edge_to_base<Vertex>, weight<Weight>
{
};

template <std::signed_integral Vertex, typename Weight>
struct weighted_edge : edge_base<Vertex>, weight<Weight>
{
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

template <typename T>
struct is_no_cv_basic_unweighted_graph : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_basic_unweighted_graph_v =
  is_no_cv_basic_unweighted_graph<T>::value || is_no_cv_unweighted_graph_v<T>;

template <typename T>
concept basic_unweighted_graph = is_no_cv_basic_unweighted_graph_v<std::remove_cvref_t<T>>;

template <typename T>
struct is_no_cv_basic_weighted_graph : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_basic_weighted_graph_v =
  is_no_cv_basic_weighted_graph<T>::value || is_no_cv_weighted_graph_v<T>;

template <typename T>
concept basic_weighted_graph = is_no_cv_basic_weighted_graph_v<std::remove_cvref_t<T>>;

template <typename T>
concept basic_graph = basic_unweighted_graph<T> || basic_weighted_graph<T>;

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

template <typename T>
struct is_no_cv_basic_edge_list : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_basic_edge_list_v = is_no_cv_basic_edge_list<T>::value || is_no_cv_edge_list_v<T>;

template <typename T>
concept basic_edge_list = is_no_cv_basic_edge_list_v<std::remove_cvref_t<T>>;

} // namespace fmia::meta

namespace fmia::graph {

template <typename Vertex, typename Edge, typename Degree>
class basic_edge_list_base
{
public:
  using vertex_type = Vertex;
  using edge_type = Edge;
  using degree_type = Degree;

protected:
  using storage_type_ = std::vector<Edge>;
  using storage_size_type_ = storage_type_::size_type;

  storage_type_ edges_;

public:
  [[nodiscard]] constexpr bool empty() const noexcept { return edges_.empty(); }

  [[nodiscard]] constexpr auto edge_size() const noexcept { return edges_.size(); }

  [[nodiscard]] constexpr auto begin() noexcept { return edges_.begin(); }
  [[nodiscard]] constexpr auto begin() const noexcept { return edges_.begin(); }

  [[nodiscard]] constexpr auto end() noexcept { return edges_.end(); }
  [[nodiscard]] constexpr auto end() const noexcept { return edges_.end(); }

  [[nodiscard]] constexpr auto& operator [](storage_size_type_ idx) { return edges_[idx]; }
  [[nodiscard]] constexpr auto& operator [](storage_size_type_ idx) const { return edges_[idx]; }

  constexpr void reserve(storage_size_type_ capacity) { edges_.reserve(capacity); }
};

template <typename Vertex, typename Edge, typename Degree>
class edge_list_base : public basic_edge_list_base<Vertex, Edge, Degree>
{
private:
  using base_ = basic_edge_list_base<Vertex, Edge, Degree>;

protected:
  std::unordered_map<Vertex, bool> exist_;

public:
  [[nodiscard]] constexpr auto vertex_size() const noexcept { return static_cast<Vertex>(exist_.size()); }
};

} // namespace fmia::graph

export namespace fmia::graph {

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree = meta::make_higher_precision_t<Vertex>>
struct basic_unweighted_edge_list : public basic_edge_list_base<Vertex, unweighted_edge<Vertex, Degree>, Degree>
{
  constexpr void add_edge(Vertex u, Vertex v) { this->edges_.emplace_back(u, v, this->edge_size()); }
};

template <
  std::signed_integral Vertex, meta::arithmetic Weight,
  meta::precision_gteq<Vertex> Degree = meta::make_higher_precision_t<Vertex>
>
struct basic_weighted_edge_list : public basic_edge_list_base<Vertex, weighted_edge<Vertex, Weight>, Degree>
{
  using weight_type = Weight;

  constexpr void add_edge(Vertex u, Vertex v, const Weight& w) { this->edges_.emplace_back(u, v, w); }
  constexpr void add_edge(Vertex u, Vertex v, Weight&& w) { this->edges_.emplace_back(u, v, std::move(w)); }
};

} // namespace fmia::graph

export namespace fmia::meta {

template <typename Vertex, typename Degree>
struct is_no_cv_basic_edge_list<graph::basic_unweighted_edge_list<Vertex, Degree>> : std::true_type
{
};

template <typename Vertex, typename Weight, typename Degree>
struct is_no_cv_basic_edge_list<graph::basic_weighted_edge_list<Vertex, Weight, Degree>> : std::true_type
{
};

} // namespace fmia::meta

export namespace fmia::graph {

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree = meta::make_higher_precision_t<Vertex>>
class unweighted_edge_list : public edge_list_base<Vertex, unweighted_edge<Vertex, Degree>, Degree>
{
public:
  constexpr void add_edge(Vertex u, Vertex v)
  {
    this->exist_[u] = this->exist_[v] = true;
    this->edges_.emplace_back(u, v, this->edge_size());
  }
};

template <
  std::signed_integral Vertex, meta::arithmetic Weight,
  meta::precision_gteq<Vertex> Degree = meta::make_higher_precision_t<Vertex>
> class weighted_edge_list : public edge_list_base<Vertex, weighted_edge<Vertex, Weight>, Degree>
{
public:
  using weight_type = Weight;

  constexpr void add_edge(Vertex u, Vertex v, const Weight& w)
  {
    this->exist_[u] = this->exist_[v] = true;
    this->edges_.emplace_back(u, v, w);
  }

  constexpr void add_edge(Vertex u, Vertex v, Weight&& w)
  {
    this->exist_[u] = this->exist_[v] = true;
    this->edges_.emplace_back(u, v, std::move(w));
  }
};

} // namespace fmia::graph

export namespace fmia::meta {

template <typename Vertex>
struct is_no_cv_edge_list<graph::unweighted_edge_list<Vertex>> : std::true_type
{
};

template <typename Vertex, typename Weight>
struct is_no_cv_edge_list<graph::weighted_edge_list<Vertex, Weight>> : std::true_type
{
};

} // namespace fmia::meta
