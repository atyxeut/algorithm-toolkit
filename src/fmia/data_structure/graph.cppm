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

export module fmia.data_structure.graph;

export import fmia.data_structure.graph.storage;

import std;

import fmia.math.integer.core;

namespace fmia::graph::shortest_path {

export enum class error : u8 {
  empty_graph,
  has_negative_cycle
};

namespace detail {

// brief explaination for correctness:
// 1. the shortest path can have at most V - 1 edges
// 2. distance[u] + w < distance[v] only happens if distance[u] was updated last round, so the rounds can be seen as steps on the shortest path
//    for example, in the 1st round, only distance[s] can relax it's neighbors (move 1 step out),
//    then in the 2nd round, choose to move 1 step to another vertex, then back (relax), cost 2 steps in total, or not do so (not relax)...
//    we can walk through all V - 1 edges in V - 1 steps, so after V - 1 rounds, distance[i] for all vertexes are found
//
// time complexity: O(VE)
template <typename EdgeList, typename Vertex, typename Weight>
constexpr auto bellman_ford_naive_impl(const EdgeList& edges, Vertex source, Vertex vertex_count, const Weight& inf) -> std::expected<std::vector<Weight>, error>
{
  if (edges.empty())
    return std::unexpected(error::empty_graph);

  std::vector<Weight> distance(vertex_count, inf);
  distance[source] = 0;

  bool relaxed;
  do {
    if (vertex_count-- == 0)
      return std::unexpected(error::has_negative_cycle);

    relaxed = false;
    for (const auto& [u, v, w] : edges) {
      // 1. avoid addition overflowl
      // 2. prevent fake path from source
      if (distance[u] == inf)
        continue;

      if (auto new_distance = distance[u] + w; new_distance < distance[v]) {
        distance[v] = new_distance;
        relaxed = true;
      }
    }
  } while (relaxed);

  return distance;
}

} // namespace detail

export template <typename Vertex, typename Weight>
[[nodiscard]] constexpr auto bellman_ford_naive(const basic_weighted_edge_list<Vertex, Weight>& edges, Vertex source, Vertex vertex_count, const Weight& inf = graph::default_weight_infinity<Weight>)
{
  return detail::bellman_ford_naive_impl(edges, source, vertex_count, inf);
}

export template <typename Vertex, typename Weight>
[[nodiscard]] constexpr auto bellman_ford_naive(const weighted_edge_list<Vertex, Weight>& edges, Vertex source, const Weight& inf = graph::default_weight_infinity<Weight>)
{
  return detail::bellman_ford_naive_impl(edges, source, edges.vertex_size(), inf);
}

// optimization: for every round, only check edges whose vertexes was relaxed in the last round (does not affect the worst case asymptotic time complexity)
//
// for correctness, any container can be used to mark the information,
// however, FIFO containers (e.g. queue) are significantly better than FILO containers (e.g. stack),
// because with an FIFO container, vertexes that are closer to the source vertex are prioritized,
// which is always better, as they are naturally more optimal than farther vertexes
//
// moreover, introducing an FILO container can cause even worse performance than the naive version,
// because useless sub-optimal results can be repeatedly propagated very much
export template <meta::graph T, std::same_as<typename T::vertex_type> Vertex, typename Weight = T::weight_type>
[[nodiscard]] constexpr auto bellman_ford_queue_optimized(const T& g, Vertex source, const Weight& inf = graph::default_weight_infinity<Weight>) -> std::expected<std::vector<Weight>, error>
{
  if (g.empty())
    return std::unexpected(error::empty_graph);

  Vertex vertex_count = g.vertex_size();

  std::vector<Weight> distance(vertex_count, inf);
  distance[source] = 0;

  std::deque<Vertex> q {source};

  // in_queue[i] < 0:  vertex i is in the queue
  // in_queue[i] >= 0: vertex i is not in the queue
  // abs(in_queue[i]): vertex i's enqueue count
  std::vector<meta::make_signed_t<Vertex>> in_queue(vertex_count);
  in_queue[source] = -1;

  while (!q.empty()) {
    auto u = q.front();
    q.pop_front();
    in_queue[u] = -in_queue[u];

    for (const auto& [v, w] : g[u].neighbors()) {
      // distance[u] never equals to inf, since only relaxed vertexes will be added to the queue
      if (auto new_distance = distance[u] + w; new_distance < distance[v]) {
        distance[v] = new_distance;
        if (in_queue[v] < 0)
          continue;
        if (in_queue[v] >= vertex_count)
          return std::unexpected(error::has_negative_cycle);
        ++in_queue[v];
        in_queue[v] = -in_queue[v];
        q.emplace_back(v);
      }
    }
  }

  return distance;
}

} // namespace fmia::graph::shortest_path
