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

export module fmia.data_structure.graph.connectivity;

import std;

import fmia.data_structure.graph.storage;

export namespace fmia::graph {

template <meta::graph T, typename V = T::vertex_type>
[[nodiscard]] constexpr std::vector<V> get_scc_tarjan_recursive(const T& g)
{
  std::vector<V> dfn(g.vertex_size()), low(g.vertex_size()), scc_id(g.vertex_size());
  V cur_dfn = 0, scc_size = 0;

  std::vector<V> stk;

  const auto dfs = [&](this const auto& self, V u) -> void {
    low[u] = dfn[u] = ++cur_dfn;
    stk.push_back(u);

    for (const auto v : g.neighbors(u)) {
      if (scc_id[v] != 0)
        continue;
      if (dfn[v] != 0) {
        self(v);
        low[u] = std::min(low[u], low[v]);
      } else {
        low[u] = std::min(low[u], dfn[v]);
      }
    }

    if (low[u] == dfn[u]) {
      ++scc_size;
      V v;
      do {
        v = stk.back();
        stk.pop_back();
        scc_id[v] = scc_size;
      } while (v != u);
    }
  };

  for (const auto u : g.vertices())
    if (dfn[u] == 0)
      dfs(u);

  return scc_id;
}

} // export namespace fmia::graph
