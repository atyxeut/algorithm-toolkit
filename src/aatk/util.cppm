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

export module aatk.util;

import std;

export import aatk.util.misc;
export import aatk.util.stl_helper;

namespace aatk {

// same values map to the same rank
export template <std::ranges::forward_range Range>
[[nodiscard]] constexpr auto compress_coordinates(const Range& range)
{
  std::vector<std::ranges::range_value_t<Range>> tmp(std::ranges::begin(range), std::ranges::end(range));
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
export template <std::ranges::input_range Range>
[[nodiscard]] constexpr auto compress_coordinates_to_unique(Range&& range)
{
  const auto n = std::ranges::size(range);
  std::vector<std::pair<std::ranges::range_value_t<Range>, std::size_t>> tmp;
  tmp.reserve(n);
  for (std::size_t i = 0; auto&& elem : range) {
    if constexpr (std::is_rvalue_reference_v<Range>)
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

} // namespace aatk
