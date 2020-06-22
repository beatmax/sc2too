#pragma once

#include <algorithm>
#include <chrono>
#include <thread>

namespace util::detail {
  template<typename Clock>
  struct RealClock : Clock {
    static void sleepUntil(const typename Clock::time_point& t) {
      std::this_thread::sleep_until(t);
    }
  };

  template<typename Clock>
  struct FakeClock {
    using time_point = typename Clock::time_point;
    using Duration = std::chrono::microseconds;

    static time_point time;
    static Duration step;

    static time_point now() { return time += step; }
    static void sleepUntil(time_point t) { time = std::max(time, t); }
  };

  template<typename Clock>
  typename FakeClock<Clock>::time_point FakeClock<Clock>::time;
  template<typename Clock>
  typename FakeClock<Clock>::Duration FakeClock<Clock>::step;
}

namespace util {
  using RealClock = detail::RealClock<std::chrono::steady_clock>;
  using FakeClock = detail::FakeClock<std::chrono::steady_clock>;
}
