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

export module aatk.debug.benchmark;

import std;

namespace aatk::benchmark {

export template <typename TRep, typename TPeriod = std::milli, typename TOtherRep, typename TOtherPeriod>
void print_duration_as(std::chrono::duration<TOtherRep, TOtherPeriod> duration, bool endline = true)
{
  const auto dur = std::chrono::duration_cast<std::chrono::duration<TRep, TPeriod>>(duration);
  if constexpr (std::floating_point<TRep>)
    std::cerr << std::fixed << std::setprecision(3) << dur;
  else
    std::cerr << dur;

  if (endline)
    std::println(std::cerr);
}

export template <typename TRep, typename TPeriod>
void print_duration(std::chrono::duration<TRep, TPeriod> duration, bool endline = true)
{
  print_duration_as<TRep, TPeriod>(duration, endline);
}

template <typename TRep, typename TPeriod, typename TResult>
struct timed_invocation_result
{
  std::chrono::duration<TRep, TPeriod> duration;
  TResult result;
};

template <typename TRep, typename TPeriod>
struct timed_invocation_result<TRep, TPeriod, void>
{
  std::chrono::duration<TRep, TPeriod> duration;
};

export template <typename TFn, typename... TArgs>
[[nodiscard]] auto timed_invocation(TFn&& func, TArgs&&... args)
{
  using time_point_type = std::chrono::steady_clock::time_point;
  using duration_type = time_point_type::duration;
  using result_type = std::invoke_result_t<TFn, TArgs...>;

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
      std::invoke(std::forward<TFn>(func), std::forward<TArgs>(args)...);
      controller.end_timer();
      return timed_invocation_result<duration_type::rep, duration_type::period, void> {timer_end - timer_begin};
    }
    else {
      auto result = std::invoke(std::forward<TFn>(func), std::forward<TArgs>(args)...);
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
