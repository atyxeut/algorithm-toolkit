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

export module aatk.debug.benchmark;

import std;

namespace aatk::benchmark {

export template <typename Rep, typename Period = std::milli, typename OtherRep, typename OtherPeriod>
void print_duration_as(std::chrono::duration<OtherRep, OtherPeriod> duration, bool endline = true)
{
  const auto dur = std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(duration);
  if constexpr (std::floating_point<Rep>)
    std::cerr << std::fixed << std::setprecision(3) << dur;
  else
    std::cerr << dur;

  if (endline)
    std::println(std::cerr);
}

export template <typename Rep, typename Period>
void print_duration(std::chrono::duration<Rep, Period> duration, bool endline = true)
{
  print_duration_as<Rep, Period>(duration, endline);
}

template <typename Rep, typename Period, typename TResult>
struct timed_invocation_result
{
  std::chrono::duration<Rep, Period> duration;
  TResult result;
};

template <typename Rep, typename Period>
struct timed_invocation_result<Rep, Period, void>
{
  std::chrono::duration<Rep, Period> duration;
};

export template <typename Fn, typename... Args>
[[nodiscard]] auto timed_invocation(Fn&& func, Args&&... args)
{
  using time_point_type = std::chrono::steady_clock::time_point;
  using duration_type = time_point_type::duration;
  using result_type = std::invoke_result_t<Fn, Args...>;

  struct timer_controller
  {
    time_point_type& timer_end_ref;
    bool is_timer_ended = false;

    void end_timer() noexcept
    {
      if (!is_timer_ended) {
        timer_end_ref = std::chrono::steady_clock::now();
        is_timer_ended = true;
      }
    }

    // when the function call throws, set the end time point before reaching the catch block
    ~timer_controller() noexcept { end_timer(); }
  };

  time_point_type timer_begin;
  time_point_type timer_end;

  try {
    timer_controller controller {timer_end};
    timer_begin = std::chrono::steady_clock::now();
    if constexpr (std::is_void_v<result_type>) {
      std::invoke(std::forward<Fn>(func), std::forward<Args>(args)...);
      controller.end_timer();
      return timed_invocation_result<duration_type::rep, duration_type::period, void> {timer_end - timer_begin};
    }
    else {
      auto result = std::invoke(std::forward<Fn>(func), std::forward<Args>(args)...);
      controller.end_timer();
      return timed_invocation_result {timer_end - timer_begin, std::move(result)};
    }
  }
  catch (...) {
    std::println(std::cerr, "exception caught, execution time before exception is ");
    print_duration_as<double>(timer_end - timer_begin);
    std::println(std::cerr);
    throw;
  }
}

export class stopwatch
{
  using time_point_type_ = std::chrono::steady_clock::time_point;

  time_point_type_ begin_time_point_;
  std::vector<time_point_type_> laps_;

public:
  stopwatch() : begin_time_point_(std::chrono::steady_clock::now()), laps_(1, begin_time_point_) {}

  void reset()
  {
    begin_time_point_ = std::chrono::steady_clock::now();
    laps_ = std::vector<time_point_type_>(1, begin_time_point_);
  }

  [[nodiscard]] auto time_since_epoch() const { return std::chrono::steady_clock::now() - begin_time_point_; }

  void lap() { laps_.emplace_back(std::chrono::steady_clock::now()); }

  [[nodiscard]] constexpr auto lap_count() const { return laps_.size() - 1; }

  void print_lap(std::size_t idx) const
  {
    if (idx == 0 || idx >= laps_.size())
      throw std::invalid_argument(std::format("invalid index range, index starts at 1, and now there are {} laps", lap_count()));

    print_duration_as<double>(laps_[idx] - laps_[idx - 1]);
  }

  // default to print the last lap
  void print_lap() const { print_lap(lap_count()); }

  void print_laps(std::size_t from_idx, std::size_t to_idx) const
  {
    if (from_idx == 0 || from_idx > to_idx || to_idx >= laps_.size())
      throw std::invalid_argument(std::format("invalid index range, index starts at 1, and now there are {} laps", lap_count()));

    for (; from_idx <= to_idx; ++from_idx) {
      std::println(std::cerr, "lap {}: ", from_idx);
      print_duration_as<double>(laps_[from_idx] - laps_[from_idx - 1]);
    }
  }

  // default to print all laps
  void print_laps() const { print_laps(1, lap_count()); }
};

} // namespace aatk::benchmark
