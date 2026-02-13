// Copyright 2026 atyxeut
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

export module aty.gatk.util;

import std;

export import aty.gatk.util.misc;
export import aty.gatk.util.stl_helper;
export import aty.gatk.util.tmp;

namespace aty::gatk {

// same values map to the same rank
export template <std::ranges::forward_range TRange>
[[nodiscard]] constexpr auto compress_coordinates(const TRange& range)
{
  std::vector<std::ranges::range_value_t<TRange>> tmp(std::ranges::begin(range), std::ranges::end(range));
  std::ranges::sort(tmp);
  const auto [tmp_end, _] = std::ranges::unique(tmp);

  const auto n = std::ranges::size(range);
  std::vector<int> rank(n);
  const auto tmp_begin = std::ranges::begin(tmp);
  for (std::size_t i = 0; const auto& elem : range)
    rank[i++] = static_cast<int>(std::ranges::lower_bound(tmp_begin, tmp_end, elem) - tmp_begin);

  return rank;
}

// every value maps to a unique rank, smaller index in the original range maps to a lower rank
export template <std::ranges::input_range TRange>
[[nodiscard]] constexpr auto compress_coordinates_to_unique(TRange&& range)
{
  const auto n = std::ranges::size(range);
  std::vector<std::pair<std::ranges::range_value_t<TRange>, std::size_t>> tmp;
  tmp.reserve(n);
  for (std::size_t i = 0; auto&& elem : range) {
    if constexpr (std::is_rvalue_reference_v<TRange>)
      tmp.emplace_back(std::move(elem), i++);
    else
      tmp.emplace_back(elem, i++);
  }
  std::ranges::sort(tmp);

  std::vector<int> rank(n);
  for (std::size_t i = 0; i < n; ++i)
    rank[tmp[i].second] = static_cast<int>(i);

  return rank;
}

} // namespace aty::gatk
