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

export module fmia.random;

import std;

import fmia.data_structure.graph.storage;
import fmia.math.integer;

export namespace fmia::random {

inline const auto time_seed = std::chrono::steady_clock::now().time_since_epoch().count();
inline std::random_device seed_generator;

inline std::mt19937 mt19937_engine(seed_generator());
inline std::mt19937_64 mt19937_64_engine(seed_generator());

template <std::integral T>
[[nodiscard]] auto uniform_distribution(T l, T r)
{
  return std::uniform_int_distribution<T>(l, r);
}

template <std::floating_point T>
[[nodiscard]] auto uniform_distribution(T l, T r)
{
  return std::uniform_real_distribution<T>(l, r);
}

template <typename T, typename Engine = std::mt19937>
  requires std::is_arithmetic_v<T>
[[nodiscard]] T rand(T l, T r, Engine& engine = mt19937_engine)
{
  return uniform_distribution(l, r)(engine);
}

} // namespace fmia::random

namespace fmia::random::generate::detail {

constexpr char decimal_digit_character[10] {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

} // namespace fmia::random::generate::detail

export namespace fmia::random::generate {

template <bool OnlyPositiveInteger = false>
[[nodiscard]] auto integer(usize integer_length)
{
  std::string data;
  data.reserve(integer_length);

  if (!OnlyPositiveInteger & rand(0uz, 1uz))
    data += '-';

  auto dist = uniform_distribution(0uz, 9uz);
  data += detail::decimal_digit_character[rand(1uz, 9uz)];
  while (integer_length-- > 1)
    data += detail::decimal_digit_character[dist(mt19937_engine)];

  return data;
}

template <std::integral T>
[[nodiscard]] auto permutation(T begin, T end)
{
  if (begin > end)
    throw std::invalid_argument("invalid integer range");

  std::vector<T> data(end - begin + 1);
  std::iota(data.begin(), data.end(), begin);
  std::ranges::shuffle(data, mt19937_engine);
  return data;
}

// get the edge list of a random unweighted tree
template <bool FlowerGraph = false, std::integral T>
[[nodiscard]] auto unweighted_tree(T vertex_begin, T vertex_end)
{
  if (vertex_begin > vertex_end)
    throw std::invalid_argument("invalid vertex index range");

  const auto p = permutation(vertex_begin, vertex_end);

  const auto edge_cnt = vertex_end - vertex_begin;
  graph::basic_unweighted_edge_list<T> data;
  data.reserve(edge_cnt);

  for (auto v = 1uz; v <= edge_cnt; ++v)
  {
    if constexpr (FlowerGraph)
      data.add_edge(p[0], p[v]);
    else
      data.add_edge(p[rand(0uz, v - 1)], p[v]);
  }

  return data;
}

// get the edge list of a random weighted tree
template <bool FlowerGraph = false, std::integral Vertex, std::integral Weight>
[[nodiscard]] auto weighted_tree(Vertex vertex_begin, Vertex vertex_end, Weight weight_begin, Weight weight_end)
{
  if (vertex_begin > vertex_end)
    throw std::invalid_argument("invalid vertex index range");

  if (weight_begin > weight_end)
    throw std::invalid_argument("invalid weight range");

  const auto p = permutation(vertex_begin, vertex_end);

  const auto edge_cnt = vertex_end - vertex_begin;
  graph::basic_weighted_edge_list<Vertex, Weight> data;
  data.reserve(edge_cnt);

  auto w_dist = uniform_dist(weight_begin, weight_end);
  for (auto v = 1uz; v <= edge_cnt; ++v)
  {
    if constexpr (FlowerGraph)
      data.add_edge(p[0], p[v], w_dist(mt19937_engine));
    else
      data.add_edge(p[rand(0uz, v - 1)], p[v], w_dist(mt19937_engine));
  }

  return data;
}

} // namespace fmia::random::generate
