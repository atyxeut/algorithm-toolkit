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

#include <macro/wconversion_switch.hpp>

export module fmia.math.integer.arbitrary_precision;

import std;

import fmia.math.integer.fixed_precision;

export namespace fmia::meta {

template <typename>
struct is_no_cv_arbitrary_precision_integral : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_arbitrary_precision_integral_v = is_no_cv_arbitrary_precision_integral<T>::value;

template <typename T>
using is_arbitrary_precision_integral = is_no_cv_arbitrary_precision_integral<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_arbitrary_precision_integral_v = is_arbitrary_precision_integral<T>::value;

template <typename T>
concept arbitrary_precision_integral = is_arbitrary_precision_integral_v<T>;

template <typename T>
concept signed_integral = twos_complement_signed_integral<T> || arbitrary_precision_integral<T>;

template <typename T>
concept integral = fixed_precision_integral<T> || arbitrary_precision_integral<T>;

template <typename T>
concept nonbool_integral = integral<T> && !boolean<T>;

} // export namespace fmia::meta

export namespace fmia::big_integer::naive {

// +, -, *, /, %, power, root operations for nonnegative operands with free functions

using mag_type = std::vector<int>;

void print(const mag_type& num, bool new_line = false)
{
  for (auto i = num.size(); i > 0;)
    std::cout << num[--i];
  if (new_line)
    std::cout << '\n';
}

[[nodiscard]] constexpr mag_type to_big_integer(std::string_view s)
{
  const auto n = s.size();
  mag_type ret(n);

  for (auto i = 0uz; i < n; ++i)
    ret[i] = s[n - i - 1] - '0';

  return ret;
}

[[nodiscard]] constexpr bool is_zero(const mag_type& num) noexcept
{
  return num.size() == 1 && num[0] == 0;
}

// a < b : -1, a = b: 0, a > b: 1
[[nodiscard]] constexpr int compare(std::span<const int> a, std::span<const int> b) noexcept
{
  const auto la = a.size(), lb = b.size();

  if (la == lb) {
    for (auto i = la; i > 0;) {
      --i;
      if (a[i] != b[i])
        return (a[i] > b[i]) - (a[i] < b[i]);
    }
  }

  return (la > lb) - (la < lb);
}

constexpr void remove_lz(mag_type& num) noexcept
{
  while (num.size() > 1 && num.back() == 0)
    num.pop_back();
}

constexpr void carry_positive(std::span<int> num) noexcept
{
  for (int c = 0; auto& digit : num) {
    digit += c;
    c = digit / 10;
    digit %= 10;
  }
}

constexpr void carry(std::span<int> num) noexcept
{
  for (int c = 0, r; auto& digit : num) {
    digit += c;
    r = digit % 10;
    c = digit / 10 - (r < 0);
    digit = r + 10 * (r < 0);
  }
}

[[nodiscard]] constexpr mag_type add(const mag_type& a, const mag_type& b)
{
  mag_type ans(std::max(a.size(), b.size()) + 1);

  for (auto i = 0uz; i < a.size(); ++i)
    ans[i] += a[i];
  for (auto i = 0uz; i < b.size(); ++i)
    ans[i] += b[i];

  carry_positive(ans);
  remove_lz(ans);
  return ans;
}

struct sub_result
{
  int sgn;
  mag_type mag;
};

void print(const sub_result& result, bool new_line = false)
{
  if (result.sgn < 0)
    std::cout << '-';
  print(result.mag, new_line);
}

[[nodiscard]] constexpr sub_result sub(const mag_type& a, const mag_type& b)
{
  mag_type ans(std::max(a.size(), b.size()));

  const int sgn = compare(a, b);

  for (auto i = 0uz; i < a.size(); ++i)
    ans[i] += a[i] * sgn;
  for (auto i = 0uz; i < b.size(); ++i)
    ans[i] -= b[i] * sgn;

  carry(ans);
  remove_lz(ans);
  return {sgn, std::move(ans)};
}

[[nodiscard]] constexpr mag_type mul(const mag_type& a, const mag_type& b)
{
  if (is_zero(a) || is_zero(b))
    return mag_type {0};

  // delayed carry is always safe here, ans[k] accumulates at most min(la, lb) additions, assume that every addition is
  // ans[k] += 9 * 9, it still requires (2^31 - 1) / 81 > 1e7 additions to overflow, in such cases, the inputs are far
  // beyond the capability of this O(n^2) algorithm
  mag_type ans(a.size() + b.size());

  for (auto i = 0uz; i < a.size(); ++i)
    for (auto j = 0uz; j < b.size(); ++j)
      ans[i + j] += a[i] * b[j];

  carry_positive(ans);
  remove_lz(ans);
  return ans;
}

FMIA_WCONVERSION_PUSH()

template <typename Remainder>
struct idiv_result
{
  mag_type q;
  Remainder r;
};

// used when b is way smaller than a
[[nodiscard]] constexpr idiv_result<int> idiv(const mag_type& a, const int& b)
{
  assert(b != 0);

  mag_type q(a.size());
  long long r = 0;

  for (auto i = q.size(); i > 0;) {
    --i;
    r = r * 10 + a[i];
    q[i] = r / b;
    r %= b;
  }

  remove_lz(q);
  return {std::move(q), static_cast<int>(r)};
}

[[nodiscard]] constexpr idiv_result<mag_type> idiv(const mag_type& a, const mag_type& b)
{
  assert(!is_zero(b));

  const int cmp_result = compare(a, b);
  if (cmp_result < 0)
    return {{0}, a};
  if (cmp_result == 0)
    return {{1}, {0}};

  mag_type q(a.size() - b.size() + 1), r(a);

  bool not_first_digit = false;
  for (auto i = q.size(); i > 0;) {
    --i;
    while ((not_first_digit && r[i + b.size()] != 0) || compare(std::span(r.begin() + i, b.size()), b) >= 0) {
      ++q[i];
      for (auto j = 0uz; j < b.size(); ++j)
        r[i + j] -= b[j];
      carry(std::span(r.begin() + i, b.size() + not_first_digit));
    }
    not_first_digit = true;
  }

  remove_lz(q);
  r.resize(b.size());
  remove_lz(r);
  return {std::move(q), std::move(r)};
}

FMIA_WCONVERSION_POP()

} // export namespace fmia::big_integer::naive
