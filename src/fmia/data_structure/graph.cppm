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

import fmia.math;

namespace fmia::graph::shortest_path {

export enum class error : std::uint8_t { empty_graph, negative_cycle };

} // namespace fmia::graph::shortest_path

namespace fmia::graph::shortest_path::single_source {

namespace detail {

// brief explaination for correctness:
// 1. the shortest path can have at most V - 1 edges
// 2. at the end of every round, if distance[i] is relaxed, then the length of the shortest path from s to i is
//    guaranteed to be increased by at least 1 (as if update layer by layer)
// as a result, V - 1 rounds are enough for every distance[i]
//
// time complexity: O(VE)
template <typename EdgeList, typename Vertex, typename Weight = EdgeList::weight_type>
constexpr auto bellman_ford_impl(const EdgeList& edges, Vertex vertex_count, Vertex source)
  -> std::expected<std::vector<Weight>, error>
{
  if (edges.empty())
    return std::unexpected(error::empty_graph);

  std::vector<Weight> distance(vertex_count, Weight::infinity);
  distance[source] = 0;

  bool relaxation_happened;
  do
  {
    // works as a round counter
    if (vertex_count-- == 0)
      return std::unexpected(error::negative_cycle);

    relaxation_happened = false;
    for (const auto& [u, v, w] : edges)
    {
      // 1. prevent fake paths from the source vertex
      // 2. avoid addition overflow
      if (distance[u] == Weight::infinity)
        continue;

      if (const auto new_distance = distance[u] + w; new_distance < distance[v])
      {
        distance[v] = new_distance;
        relaxation_happened = true;
      }
    }
  } while (relaxation_happened);

  return distance;
}

} // namespace detail

export template <typename Vertex, typename Weight>
[[nodiscard]] constexpr auto bellman_ford(
  const basic_weighted_edge_list<Vertex, Weight>& edges, Vertex vertex_count, Vertex source
)
{
  return detail::bellman_ford_impl(edges, vertex_count, source);
}

export template <typename Vertex, typename Weight>
[[nodiscard]] constexpr auto bellman_ford(const weighted_edge_list<Vertex, Weight>& edges, Vertex source)
{
  return detail::bellman_ford_impl(edges, edges.vertex_size(), source);
}

// Moore's variation:
// only check vertices that were relaxed in the last round (obviously correct)
//
// the choice of the container to hold the vertices can affect performance, some of them can even make the algorithm
// worse than the vanilla version, for example, using a stack causes worst case exponential time complexity, this is
// because that the process becomes depth first, i.e. if the optimal relaxation is performed first, then a suboptimal
// relaxation is instead propagated first
//
// this implementation uses a queue to hold the vertices, guarantees to be not worse than the vanilla bellman-ford,
// because a normal queue does not affect the original update order of the bellman-ford, and is faster in average cases
export template <meta::graph T, typename Vertex = T::vertex_type, typename Weight = T::weight_type>
[[nodiscard]] constexpr auto bellman_ford_queue_optimized(const T& g, Vertex source)
  -> std::expected<std::vector<Weight>, error>
{
  if (g.empty())
    return std::unexpected(error::empty_graph);

  const auto vertex_count = g.vertex_size();

  std::vector<Weight> distance(vertex_count, Weight::infinity);
  distance[source] = 0;

  // path_length[i] < 0: vertex i is in the queue
  // path_length[i] >= 0: vertex i is not in the queue
  // abs(path_length[i]): (edge size of the shortest path from the souce vertex to vertex i) + 1
  std::vector<meta::make_signed_t<Vertex>> path_length(vertex_count);
  path_length[source] = -1;

  std::deque<Vertex> q {source};
  while (!q.empty())
  {
    const auto u = q.front();
    q.pop_front();
    path_length[u] = -path_length[u];

    // in case u has a self loop
    const auto cur_length = path_length[u];

    for (const auto& [v, w] : g.neighbors(u))
    {
      // distance[u] never equals to infinity, since only relaxed vertices are added to the queue
      if (const auto new_distance = distance[u] + w; new_distance < distance[v])
      {
        distance[v] = new_distance;

        const bool v_in_queue = path_length[v] < 0;
        path_length[v] = cur_length + 1;

        // in this pure queue optimized version, an enqueue_count array also works for detecting negative cycles, since
        // it doesn't break the breadth first nature of the bellman-ford algorithm, however, the performance would be
        // worse, because the algorithm may have to traverse the cycle multiple times to get enough information
        if (static_cast<Vertex>(path_length[v]) > vertex_count)
          return std::unexpected(error::negative_cycle);

        path_length[v] = -path_length[v];

        if (!v_in_queue)
          q.emplace_back(v);
      }
    }
  }

  return distance;
}

// SLF + LLL optimization using a deque (emulate a priority queue):
// small label first: when pushing a vertex i, if distance[i] < distance[front], push it front, otherwise back
// large label last: when popping a vertex, pop the vertex i that distance[i] < (average weight of the deque) first
//
// 1993
// A Simple and Fast Label Correcting Algorithm for Shortest Paths
// Dimitri P. Bertsekas
// https://web.mit.edu/dimitrib/www/SLF.pdf
//
// 1996
// Parallel Asynchronous Label-Correcting Methods for Shortest Paths
// Dimitri P. Bertsekas, Francesca Guerriero, and Roberto Musmanno
// https://web.mit.edu/dimitrib/www/parallelsp.pdf
export template <meta::graph T, typename Vertex = T::vertex_type, typename Weight = T::weight_type>
[[nodiscard]] constexpr auto bellman_ford_deque_optimized(const T& g, Vertex source)
  -> std::expected<std::vector<Weight>, error>
{
  if (g.empty())
    return std::unexpected(error::empty_graph);

  const auto vertex_count = g.vertex_size();

  std::vector<Weight> distance(vertex_count, Weight::infinity);
  distance[source] = 0;

  std::vector<meta::make_signed_t<Vertex>> path_length(vertex_count);
  path_length[source] = -1;

  using sum_type = meta::make_larger_width_t<Weight>;
  sum_type sum = 0;

  std::deque<Vertex> q {source};
  while (!q.empty())
  {
    for (const auto cnt = sum_type(q.size()); cnt * distance[q.front()] > sum;)
    {
      auto frt = q.front();
      q.pop_front();
      q.emplace_back(std::move(frt));
    }
    const auto u = q.front();
    q.pop_front();
    sum -= distance[u];
    path_length[u] = -path_length[u];
    const auto cur_length = path_length[u];

    for (const auto& [v, w] : g.neighbors(u))
    {
      if (const auto new_distance = distance[u] + w; new_distance < distance[v])
      {
        const bool v_in_queue = path_length[v] < 0;
        path_length[v] = cur_length + 1;
        if (static_cast<Vertex>(path_length[v]) > vertex_count)
          return std::unexpected(error::negative_cycle);
        path_length[v] = -path_length[v];

        if (v_in_queue)
          sum = sum - distance[v] + new_distance;
        else
          sum += new_distance;
        distance[v] = new_distance;

        if (!v_in_queue)
        {
          if (q.empty() || distance[v] >= distance[q.front()])
            q.emplace_back(v);
          else
            q.emplace_front(v);
        }
      }
    }
  }

  return distance;
}

} // namespace fmia::graph::shortest_path::single_source
