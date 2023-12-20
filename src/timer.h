#pragma once

#include <cassert>
#include <chrono>
#include <cstdint>
#include <optional>

#include "time_types.h"

namespace blunder {

template<typename T>
inline Nanoseconds
to_nanos(T dur) noexcept
{ return std::chrono::duration_cast<Nanoseconds>(dur); }

template<typename T>
inline Milliseconds
to_millis(T dur) noexcept
{ return std::chrono::duration_cast<Milliseconds>(dur); }

template<typename T>
inline Microseconds
to_micros(T dur) noexcept
{ return std::chrono::duration_cast<Microseconds>(dur); }

template<typename T>
inline Minutes
to_minutes(T dur) noexcept
{ return std::chrono::duration_cast<Minutes>(dur); }

class Timer {
public:
  // Starts timing a new time interval.
  void
  start() noexcept
  { time_point = SteadyClock::now(); }

  // Ends a time a time interval. Throws an exception if start has not been
  // called yet.
  SteadyDuration
  end();

  SteadyDuration
  total() const
  { return total_time; }

  // Getters for the total time.

  std::int64_t
  total_seconds() const
  { return total_time.count(); }

  std::int64_t
  total_millis() const
  { return to_millis(total_time).count(); }

  std::int64_t
  total_nanos() const
  { return to_nanos(total_time).count(); }

  std::int64_t
  total_micros() const
  { return to_micros(total_time).count(); }

  std::int64_t
  total_minutes() const
  { return to_minutes(total_time).count(); }

private:

  double avg(std::int64_t dur) const noexcept
  {
    assert(total_intervals && "Total intervals cannot be zero.");
    return static_cast<double>(dur) / total_intervals;
  }

public:
  // Getters for the average time.

  double
  avg_seconds() const
  { return avg(total_seconds()); }

  double
  avg_millis() const
  { return avg(total_millis()); }

  double
  avg_nanos() const
  { return avg(total_nanos()); }

  double
  avg_micros() const
  { return avg(total_micros()); }

  // Returns the total number of time intervals.
  unsigned
  num_intervals() const noexcept
  { return total_intervals; }

private:
  std::optional<SteadyTimePoint> time_point = std::nullopt;
  SteadyDuration total_time{0};
  unsigned total_intervals = 0;
};

} // namespace blunder
