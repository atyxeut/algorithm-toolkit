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

namespace fmia::graph {

enum class toposort_order : u8 { none, lexicographical };
enum class toposort_error : u8 { has_cycle };

namespace detail {

template <toposort_order Order, typename Graph, typename Fn>
constexpr auto toposort_impl(const Graph& g, Fn&& fn) -> std::expected<bool, toposort_error>
{
  using vertex_type = Graph::vertex_type;

  using queue_type = std::conditional_t<
    Order == toposort_order::lexicographical,
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

} // namespace detail

export template <meta::graph T, typename Fn>
[[nodiscard]] constexpr auto toposort(const T& g, Fn&& fn)
{
  return detail::toposort_impl<toposort_order::none>(g, std::forward<Fn>(fn));
}

export template <meta::graph T, typename Fn>
[[nodiscard]] constexpr auto toposort_lexicographical_order(const T& g, Fn&& fn)
{
  return detail::toposort_impl<toposort_order::lexicographical>(g, std::forward<Fn>(fn));
}

} // namespace fmia::graph
