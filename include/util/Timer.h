#pragma once

#include "Clock.h"

#include <chrono>

namespace util {
  template<typename Clock = RealClock>
  class Timer {
  public:
    using Time = std::chrono::microseconds;

    Timer() { reset(); }
    Time elapsed() const { return std::chrono::duration_cast<Time>(Clock::now() - start_); }
    void reset() { start_ = Clock::now(); }
    void sleepUntil(Time t) { Clock::sleepUntil(start_ + t); }

  private:
    using TimePoint = typename Clock::time_point;

    TimePoint start_;
  };
}
