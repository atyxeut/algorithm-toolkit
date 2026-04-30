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

export module fmia.data_structure.graph.traverse;

import std;

import fmia.data_structure.graph.storage;
import fmia.math;

export namespace fmia::graph {

enum class trail_tag : u8 { none, circuit };
enum class path_tag : u8 { none, cycle };

enum class eulerian_graph_error : u8 { no_eulerian_trail, no_eulerian_circuit };

} // namespace fmia::graph

namespace fmia::graph {

template <graph_tag GraphTag, typename Graph, typename Vertex = Graph::vertex_type>
constexpr auto get_eulerian_trail_start_vertex(const Graph& g) noexcept -> std::pair<Vertex, bool>
{
  const auto n = g.vertex_size();
  Vertex start = -1, end = -1;

  if constexpr (GraphTag == graph_tag::undirected) {
    for (Vertex u = 0; u < n; ++u) {
      if (g.degree(u) & 1) {
        if (start == -1)
          start = u;
        else if (end == -1)
          end = u;
        else
          return {-1, false};
      }
    }
  }

  if constexpr (GraphTag == graph_tag::directed) {
    for (Vertex u = 0; u < n; ++u) {
      const auto diff = g.in_degree(u) - g.out_degree(u);
      if (diff < -1 | diff > 1 | (diff == -1 & start != -1) | (diff == 1 & end != -1))
        return {-1, false};
      if (diff == -1)
        start = u;
      else if (diff == 1)
        end = u;
    }
  }

  // no Eulerian trails
  // for undirected graphs: missing the end
  // for directed graphs: missing the start or missing the end
  if (start == -1 ^ end == -1)
    return {-1, false};

  // has an Eulerian trail
  if (start != -1)
    return {start, false};

  // both the start and the end are -1, meaning:
  // for undirected graphs: every vertex has even degree
  // for directed graphs: for every vertex, its in degree = its out degree
  // check if there's an Eulerian circuit
  for (Vertex u = 0; u < n; ++u)
    if (g.degree(u) > 0)
      return {u, true};

  // every vertex is isolated, or the graph is a null graph, we consider it to have an empty Eulerian circuit
  return {-1, true};
}

template <typename Graph, typename Vertex = Graph::vertex_type, typename Iterator = Graph::neighbor_iterator>
constexpr auto init_current_edge_iterators(const Graph& g)
{
  const auto n = g.vertex_size();

  std::vector<Iterator> res(n);
  for (Vertex u = 0; u < n; ++u)
    res[u] = g.neighbors(u).begin();

  return res;
}

} // namespace fmia::graph

namespace fmia::graph {

// Hierholzer's algorithm
// time complexity: O(V + E)

template <typename G, typename T, typename Y, typename U, typename I>
constexpr void get_an_eulerian_trail_impl_for_undirected_recursive(const G& g, T u, Y& cur_edge_it, U& vis, I& path)
{
  for (const auto end_it = g.neighbors(u).end(); cur_edge_it[u] != end_it;) {
    if (const auto [v, id] = *cur_edge_it[u]++; !vis[id]) {
      vis[id] = true;
      get_an_eulerian_trail_impl_for_undirected_recursive(g, v, cur_edge_it, vis, path);
    }
  }
  path.emplace_back(u);
}

template <typename G, typename T, typename Y, typename U, typename I>
constexpr void get_an_eulerian_trail_impl_for_undirected_iterative(const G& g, T start, Y& cur_edge_it, U& vis, I& path)
{
  std::vector<T> stack {start};
  while (!stack.empty()) {
    const auto u = stack.back();
    if (const auto end_it = g.neighbors(u).end(); cur_edge_it[u] != end_it) {
      if (const auto [v, id] = *cur_edge_it[u]++; !vis[id]) {
        vis[id] = true;
        stack.emplace_back(v);
      }
    } else {
      path.emplace_back(u);
      stack.pop_back();
    }
  }
}

template <typename G, typename T, typename Y, typename U>
constexpr void get_an_eulerian_trail_impl_for_directed_recursive(const G& g, T u, Y& cur_edge_it, U& path)
{
  for (const auto end_it = g.neighbors(u).end(); cur_edge_it[u] != end_it;)
    get_an_eulerian_trail_impl_for_directed_recursive(g, *cur_edge_it[u]++, cur_edge_it, path);
  path.emplace_back(u);
}

template <typename G, typename T, typename Y, typename U>
constexpr void get_an_eulerian_trail_impl_for_directed_iterative(const G& g, T start, Y& cur_edge_it, U& path)
{
  std::vector<T> stack {start};
  while (!stack.empty()) {
    const auto u = stack.back();
    if (const auto end_it = g.neighbors(u).end(); cur_edge_it[u] != end_it)
      stack.emplace_back(*cur_edge_it[u]++);
    else {
      path.emplace_back(u);
      stack.pop_back();
    }
  }
}

template <graph_tag GraphTag, typename T, typename G, typename U>
constexpr void get_an_eulerian_trail_impl(T& path, const G& g, U start)
{
  auto cur_edge_it = init_current_edge_iterators(g);

  if constexpr (GraphTag == graph_tag::undirected) {
    std::vector<bool> vis(g.edge_size() >> 1);
    get_an_eulerian_trail_impl_for_undirected_iterative(g, start, cur_edge_it, vis, path);
  }

  if constexpr (GraphTag == graph_tag::directed)
    get_an_eulerian_trail_impl_for_directed_iterative(g, start, cur_edge_it, path);
}

} // namespace fmia::graph

namespace fmia::graph {

template <trail_tag TrailTag, graph_tag GraphTag, typename Graph, typename Vertex = Graph::vertex_type>
constexpr auto get_an_eulerian_trail(const Graph& g) -> std::expected<std::vector<Vertex>, eulerian_graph_error>
{
  std::vector<Vertex> path;

  const auto [start, is_circuit] = get_eulerian_trail_start_vertex<GraphTag>(g);

  if constexpr (TrailTag == trail_tag::circuit)
    if (!is_circuit)
      return std::unexpected(eulerian_graph_error::no_eulerian_circuit);

  if (start == -1) {
    if (is_circuit)
      return path;
    else
      return std::unexpected(eulerian_graph_error::no_eulerian_trail);
  }

  get_an_eulerian_trail_impl<GraphTag>(path, g, start);
  std::ranges::reverse(path);
  return path;
}

} // namespace fmia::graph

export namespace fmia::graph {

// leave isolated vertices, the rest of the graph must be (strongly) connected, otherwise the the following functions
// have undefined behavior

template <meta::graph T>
[[nodiscard]] constexpr auto get_an_eulerian_trail_for_undirected(const T& g)
{
  return get_an_eulerian_trail<trail_tag::none, graph_tag::undirected>(g);
}

template <meta::graph T>
[[nodiscard]] constexpr auto get_an_eulerian_circuit_for_undirected(const T& g)
{
  return get_an_eulerian_trail<trail_tag::circuit, graph_tag::undirected>(g);
}

template <meta::graph T>
[[nodiscard]] constexpr auto get_an_eulerian_trail_for_directed(const T& g)
{
  return get_an_eulerian_trail<trail_tag::none, graph_tag::directed>(g);
}

template <meta::graph T>
[[nodiscard]] constexpr auto get_an_eulerian_circuit_for_directed(const T& g)
{
  return get_an_eulerian_trail<trail_tag::circuit, graph_tag::directed>(g);
}

} // namespace fmia::graph

export namespace fmia::graph {

// Hamiltonian path: a path that visits each vertex exactly once
// if a graph has a Hamiltonian path, then it is called a semi-Hamiltonian graph
//
// Hamiltonian cycle: a closed Hamiltonian path
// if a graph has a Hamiltonian cycle, then it is called a Hamiltonian graph

} // namespace fmia::graph

export namespace fmia::graph {

enum class toposort_error : u8 { has_cycle };
enum class toposort_tag : u8 { none, lexicographical };

} // namespace fmia::graph

namespace fmia::graph {

template <toposort_tag Order, typename Graph, typename Fn>
constexpr auto toposort_impl(const Graph& g, Fn&& fn) -> std::expected<bool, toposort_error>
{
  using vertex_type = Graph::vertex_type;

  using queue_type = std::conditional_t<
    Order == toposort_tag::lexicographical,
    std::priority_queue<vertex_type, std::vector<vertex_type>, std::greater<vertex_type>>, std::queue<vertex_type>
  >;

  vertex_type n = g.vertex_size();

  queue_type q;
  for (vertex_type u = 0; u < n; ++u)
    if (g.in_degree(u) == 0)
      q.push(u);

  bool unique_order = true;

  while (!q.empty()) {
    if (q.size() > 1)
      unique_order = false;

    const auto u = [&] {
      if constexpr (std::same_as<queue_type, std::queue<vertex_type>>)
        return q.front();
      else
        return q.top();
    }();
    q.pop();
    --n;

    std::invoke(fn, u);

    for (const auto v : g.neighbors(u))
      if (--g.in_degree(v) == 0)
        q.push(v);
  }

  if (n != 0)
    return std::unexpected(toposort_error::has_cycle);

  return unique_order;
}

} // namespace fmia::graph

export namespace fmia::graph {

template <meta::graph T, typename Fn>
[[nodiscard]] constexpr auto toposort(const T& g, Fn&& fn)
{
  return toposort_impl<toposort_tag::none>(g, std::forward<Fn>(fn));
}

template <meta::graph T, typename Fn>
[[nodiscard]] constexpr auto toposort_lexicographical(const T& g, Fn&& fn)
{
  return toposort_impl<toposort_tag::lexicographical>(g, std::forward<Fn>(fn));
}

} // namespace fmia::graph
