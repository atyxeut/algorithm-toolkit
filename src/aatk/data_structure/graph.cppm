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

export module aatk.data_structure.graph;

import std;

import aatk.math;

namespace aatk::graph {

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

export template <std::integral Vertex, typename Weight = void>
class edge_list
{
  std::vector<edge<Vertex, Weight>> edges_;

public:
  [[nodiscard]] constexpr const auto& operator [](usize idx) const { return edges_[idx]; }
  [[nodiscard]] auto& operator [](usize idx) { return edges_[idx]; }

  constexpr auto begin() const noexcept { return edges_.begin(); }
  constexpr auto begin() noexcept { return edges_.begin(); }

  constexpr auto end() const noexcept { return edges_.end(); }
  constexpr auto end() noexcept { return edges_.end(); }

  [[nodiscard]] constexpr auto size() const noexcept { return edges_.size(); }
  constexpr void reserve(usize capacity) { edges_.reserve(capacity); }

  template <typename... Args>
  constexpr auto& add_edge(Args... args)
  {
    return edges_.emplace_back(std::forward<Args>(args)...);
  }
};

} // namespace aatk::graph
