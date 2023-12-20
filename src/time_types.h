#pragma once

#include <chrono>

namespace blunder {

using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;
using Minutes = std::chrono::minutes;

using SteadyClock = std::chrono::steady_clock;
using SteadyTimePoint = SteadyClock::time_point;
using SteadyDuration = SteadyClock::duration;

} // namespace blunder
