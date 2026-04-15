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

module;

#include <cassert>

export module fmia.math.counting;

import std;

import fmia.math.core;

namespace fmia {

namespace detail {

// count occurrences of digit x in all integers in [0, n]
// time complexity: O(log n)
template <typename T>
[[nodiscard]] constexpr auto count_digit_occurrence_impl(const T& n, int x)
{
  // the answer will easily exceed n, for example: for n = 2147483647, all answers for x in [0, 9] exceed 2147483647
  meta::make_higher_precision_t<T> ans = 0;

  for (T n_ = n, coeff = 1; n_ != 0; n_ /= 10, coeff *= 10) {
    // divide the number into 3 parts: high mid low, then count how many times does x occur at mid
    // coeff: 10^(weight of mid)
    const T high = n / (coeff * 10), mid = n_ % 10, low = n % coeff;

    // pick h from [0, high - 1], then numbers in [h _ 0...0, h _ 9...9] (_ in [0, 9]) are also in [0, n], for example,
    // n = 12345, coeff = 100 --> high = 12, mid = 3, low = 45, then numbers in intervals [0 0 00, 0 0 99], [0 1 00,
    // 0 1 99], ..., [0 9 00, 0 9 99], [1 _ 00, 1 _ 99], ..., [11 _ 00, 11 _ 99] are all in [0, n]
    //
    // that is to say, for x > 0 cases, interval [h x 0...0, h x 9...9] contributes coeff occurrences of x, and there
    // are h such intervals, so the total contribution is high * coeff, for x = 0 case, the contribution of interval
    // [0 x 0...0, 0 x 9...9] (i.e. [0...0, 9...9]) is 0, the total contribution becomes (high - 1) * coeff
    //
    // for h = high part, we only have interval [high x 0...0, high mid low], if x > mid, the contribution is 0, if
    // x = mid, the contribution is (low + 1), if x < mid, the interval is complete: [high x 0...0, high x 9...9], and
    // the contribution is coeff if high and x are not both 0, otherwise is 0

    // h in [0, high - 1]
    if (x == 0 && high != 0)
      ans += (high - 1) * coeff;
    else // (x == 0 && high == 0) and (x != 0) cases
      ans += high * coeff;

    // h = high
    if (x == mid)
      ans += low + 1;
    else if (x < mid && !(x == 0 && high == 0))
      ans += coeff;
  }

  return ans + (x == 0);
}

} // namespace detail

export template <meta::fixed_precision_integral T>
[[nodiscard]] constexpr auto count_digit_occurrence(const T& l, const T& r, int x)
{
  assert(0 <= l && l <= r && 0 <= x && x <= 9);

  if (l == 0)
    return detail::count_digit_occurrence_impl(r, x);

  return detail::count_digit_occurrence_impl(r, x) - detail::count_digit_occurrence_impl(l - 1, x);
}

// count occurrences of digit x in n
// time complexity: O(log n)
export template <meta::fixed_precision_integral T>
[[nodiscard]] constexpr auto count_digit_occurrence(T n, int x) noexcept
{
  assert(n >= 0 && 0 <= x && x <= 9);

  T ans = 0;
  while (n != 0) {
    ans += n % 10 == x;
    n /= 10;
  }

  return ans;
}

} // namespace fmia
