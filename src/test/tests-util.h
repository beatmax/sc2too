#pragma once

#include "catch2/catch.hpp"
#include "util/Pool.h"

#include <chrono>
#include <string>
#include <utility>

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

  template<typename T1, typename T2>
  struct StringMaker<std::pair<T1, T2>> {
    using Pair = std::pair<T1, T2>;
    static std::string convert(const Pair& p) {
      return "{" + StringMaker<T1>::convert(p.first) + ", " + StringMaker<T1>::convert(p.second) +
          "}";
    }
  };

  template<typename T>
  struct StringMaker<util::PoolObjectId<T>> {
    using Id = util::PoolObjectId<T>;
    static std::string convert(const Id& id) { return std::to_string(id.idx); }
  };
}
