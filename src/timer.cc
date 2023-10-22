#include "timer.h"

#include <cassert>

namespace blunder {

SteadyDuration
Timer::end()
{
  assert(time_point and "Timer::start() needs to be called before Timer::end()");
  auto dur = SteadyClock::now() - *time_point;
  total_time += dur;
  ++total_intervals;
  return dur;
}

} // namespace blunder
