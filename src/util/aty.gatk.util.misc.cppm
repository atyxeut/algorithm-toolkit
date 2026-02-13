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

export module aty.gatk.util.misc;

import std;

import aty.gatk.math.integer;

namespace aty::gatk {

namespace tower_of_hanoi {

// the 3-peg Tower of Hanoi problem
//
// solvability: any valid state with n disks can become a state where all n disks are on one peg
// proof:
//   1. n = 1, obviously solvable
//   2. assume that a valid state with n = k - 1 disks can become a state where all k - 1 disks are on one peg
//   3. for n = k,
//        if the biggest disk k is already in position, the k - 1 disks can be on the destination peg later (using the hypothesis), OK
//        otherwise, smaller k - 1 disks can be on one auxiliary peg, then disk k be on the destination, then the k - 1 disks be on the destination, OK
// thus a general form problem guarantees a solution as described above, and any other solution that has the same recursive structure is also valid

using move_cnt_type = std::uintmax_t;

// pegs are 0, 1, 2
constexpr u32 peg_cnt = 3;
constexpr auto peg_name_upper = "ABC";
constexpr auto peg_name_lower = "abc";

namespace get_move_cnt {

// initially all disks stack on one peg `from`, finally they all stack on one peg `to`
export [[nodiscard]] constexpr move_cnt_type from_one_to_one_form(u32 disk_cnt) noexcept(noexcept((move_cnt_type(1) << disk_cnt) - 1))
{
  // denote the moves of a problem regarding n disks by T(n)
  // 1. to move disk n to `to`, we can first move disk n - 1 to 1 the auxiliary (third) peg, from peg `from`
  //    the problem of moving these n - 1 disks is the same as moving all n disks, so the move count is T(n - 1)
  // 2. after, only 1 move is required for n to be on `to` peg
  // 3. then similarly, another (n - 1)-disk problem, from auxiliary peg to peg `to`, T(n - 1) more moves, so 2T(n - 1) + 1 moves guarantee a solution, T(n) <= 2T(n - 1) + 1
  //    on the other hand, T(n - 1) moves are required before moving n, so T(n) >= 2T(n - 1) + 1, then the result is T(n) = 2T(n - 1) + 1 = 2^n - 1
  return (move_cnt_type(1) << disk_cnt) - 1;

  // moreover, let f(i) be the moves of the i-th disk
  // since the largest disk of a problem only moves once, clearly f(n) = 1,
  //   and the move count for a general i is the count that the i-th subproblem is called during the recursion,
  //   draw the recursive tree, we can easily see that f(i) = 2^(n - i):
  //                       f(n)
  //          f(n - 1)              f(n - 1)
  //     f(n - 2)   f(n - 2)   f(n - 2)   f(n - 2)
  //             ...                   ...
}

// initially all disks stack scatteredly, but finally they all stack on one peg `to`
// from_list[i]: the (i + 1)-th disk's initial peg
export [[nodiscard]] constexpr move_cnt_type from_different_to_one_form(u32 disk_cnt, std::span<const u32> from_list, u32 to) noexcept(noexcept(from_one_to_one_form(disk_cnt)))
{
  if (disk_cnt == 0)
    return 0;

  // if the largest is already in position, just deal with the disks left
  const u32 from = from_list[disk_cnt - 1];
  if (from == to)
    return from_different_to_one_form(disk_cnt - 1, from_list, to);

  // 1. have to move all other disks to the auxiliary peg before the largest one is able to be moved to peg `to`
  // 2. after moving the largest one, move the remaining from the auxiliary peg to peg `to`, this problem has "from one to one" form
  const u32 aux = peg_cnt - from - to;
  return from_different_to_one_form(disk_cnt - 1, from_list, aux) + 1 + from_one_to_one_form(disk_cnt - 1);
}

// initially all disks stack on one peg `from`, but finally they stack scatteredly
// to_list[i]: the (i + 1)-th disk's destination peg
export [[nodiscard]] constexpr move_cnt_type from_one_to_different_form(u32 disk_cnt, u32 from, std::span<const u32> to_list) noexcept(noexcept(from_one_to_one_form(disk_cnt)))
{
  if (disk_cnt == 0)
    return 0;

  const u32 to = to_list[disk_cnt - 1];
  if (from == to)
    return from_one_to_different_form(disk_cnt - 1, from, to_list);

  const u32 aux = peg_cnt - from - to;
  return from_one_to_one_form(disk_cnt - 1) + 1 + from_one_to_different_form(disk_cnt - 1, aux, to_list);
}

// move the largest disk from peg `from` to peg `to` using only one step (in most cases this is optimal)
[[nodiscard]] constexpr move_cnt_type general_form_one_step_strategy(u32 disk_cnt, std::span<const u32> from_list, std::span<const u32> to_list) noexcept(noexcept(from_one_to_one_form(disk_cnt)))
{
  // 1. move disks above the largest disk n and on peg `to[n]` to peg `aux`, so that disk n can move
  // 2. move disk n to the destination, then move the remaining n - 1 disks on peg `aux` to their destinations
  const u32 aux = peg_cnt - from_list[disk_cnt - 1] - to_list[disk_cnt - 1];
  return from_different_to_one_form(disk_cnt - 1, from_list, aux) + 1 + from_one_to_different_form(disk_cnt - 1, aux, to_list);
}

// moving the largest disk n first from peg `from` to peg `aux`, then to `to`, using two steps, is sometimes better, for example:
//   initial state --> final state
//     A: 3              A: 2 1
//     B:                B:
//     C: 2 1            C: 3
// one step strategy (7 moves):
//   move 1 from C to A
//   move 2 from C to B
//   move 1 from A to B
//   move 3 from A to C (the only one move)
//   move 1 from B to A
//   move 2 from B to C
//   move 1 from A to C
// two step strategy (5 moves, better):
//   move 3 from A to B (the first move)
//   move 1 from C to B
//   move 2 from C to A
//   move 1 from B to A
//   move 3 from B to A (the second move)
[[nodiscard]] constexpr move_cnt_type general_form_two_step_strategy(u32 disk_cnt, std::span<const u32> from_list, std::span<const u32> to_list) noexcept(noexcept(from_one_to_one_form(disk_cnt)))
{
  // 1. clear disks above the largest disk n and on peg `aux`, move them to peg `to[n]`
  // 2. move disk n to peg `aux` (the first move)
  // 3. move the remaining n - 1 disks on peg `to[n]` to `from[n]`, this problem has "from one to one" form
  // 4. move disk n to peg `to[n]` (the second move)
  // 5. move the remaining n - 1 disks on peg `from[n]` to their destinations
  const u32 from = from_list[disk_cnt - 1];
  const u32 to = to_list[disk_cnt - 1];
  return from_different_to_one_form(disk_cnt - 1, from_list, to) + 1 + from_one_to_one_form(disk_cnt - 1) + 1 + from_one_to_different_form(disk_cnt - 1, from, to_list);
}

// initially all disks stack scatteredly, and finally they also stack scatteredly
export [[nodiscard]] constexpr move_cnt_type general_form(u32 disk_cnt, std::span<const u32> from_list, std::span<const u32> to_list) noexcept(noexcept(from_one_to_one_form(disk_cnt)))
{
  if (disk_cnt == 0)
    return 0;

  const u32 from = from_list[disk_cnt - 1];
  const u32 to = to_list[disk_cnt - 1];
  if (from == to)
    return general_form(disk_cnt - 1, from_list, to_list);

  return std::min(general_form_one_step_strategy(disk_cnt, from_list, to_list), general_form_two_step_strategy(disk_cnt, from_list, to_list));
}

} // namespace get_move_cnt

namespace print_moves {

void move(u32 disk, u32 from, u32 to, bool upper_case = true)
{
  const auto& peg_name = upper_case ? peg_name_upper : peg_name_lower;
  std::println("move {} from {} to {}", disk, peg_name[from], peg_name[to]);
}

export void from_one_to_one_form(u32 disk_cnt, u32 from, u32 to)
{
  if (disk_cnt == 0)
    return;

  const u32 aux = peg_cnt - from - to;
  from_one_to_one_form(disk_cnt - 1, from, aux);
  move(disk_cnt, from, to);
  from_one_to_one_form(disk_cnt - 1, aux, to);
}

export void from_different_to_one_form(u32 disk_cnt, std::span<const u32> from_list, u32 to)
{
  if (disk_cnt == 0)
    return;

  const u32 from = from_list[disk_cnt - 1];
  if (from == to) {
    from_different_to_one_form(disk_cnt - 1, from_list, to);
    return;
  }

  const u32 aux = peg_cnt - from - to;
  from_different_to_one_form(disk_cnt - 1, from_list, aux);
  move(disk_cnt, from, to);
  from_one_to_one_form(disk_cnt - 1, aux, to);
}

export void from_one_to_different_form(u32 disk_cnt, u32 from, std::span<const u32> to_list)
{
  if (disk_cnt == 0)
    return;

  const u32 to = to_list[disk_cnt - 1];
  if (from == to) {
    from_one_to_different_form(disk_cnt - 1, from, to_list);
    return;
  }

  const u32 aux = peg_cnt - from - to;
  from_one_to_one_form(disk_cnt - 1, from, aux);
  move(disk_cnt, from, to);
  from_one_to_different_form(disk_cnt - 1, aux, to_list);
}

export void general_form(u32 disk_cnt, std::span<const u32> from_list, std::span<const u32> to_list)
{
  if (disk_cnt == 0)
    return;

  const u32 from = from_list[disk_cnt - 1], to = to_list[disk_cnt - 1];
  if (from == to) {
    general_form(disk_cnt - 1, from_list, to_list);
    return;
  }

  const u32 aux = peg_cnt - from - to;
  const auto one_step_strategy_cost = get_move_cnt::general_form_one_step_strategy(disk_cnt, from_list, to_list);
  const auto two_step_strategy_cost = get_move_cnt::general_form_two_step_strategy(disk_cnt, from_list, to_list);
  if (one_step_strategy_cost <= two_step_strategy_cost) {
    from_different_to_one_form(disk_cnt - 1, from_list, aux);
    move(disk_cnt, from, to);
    from_one_to_different_form(disk_cnt - 1, aux, to_list);
  }
  else {
    from_different_to_one_form(disk_cnt - 1, from_list, to);
    move(disk_cnt, from, aux);
    from_one_to_one_form(disk_cnt - 1, to, from);
    move(disk_cnt, aux, to);
    from_one_to_different_form(disk_cnt - 1, from, to_list);
  }
}

} // namespace print_moves

} // namespace tower_of_hanoi

} // namespace aty::gatk
