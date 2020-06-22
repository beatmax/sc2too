#pragma once

#include "catch2/catch.hpp"

#include <chrono>
#include <string>

namespace test {
  struct FakeController {
    bool paused() const { return paused_; }
    bool quit() const { return quit_; }

    bool paused_{false};
    bool quit_{false};
  };
}

namespace Catch {
  template<typename Rep, typename Period>
  struct StringMaker<std::chrono::duration<Rep, Period>> {
    using Duration = std::chrono::duration<Rep, Period>;
    static std::string convert(const Duration& d) {
      return std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(d).count()) +
          "us";
    }
  };
}
