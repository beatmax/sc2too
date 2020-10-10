#pragma once

#include <functional>
#include <utility>

namespace util {
  class ScopeExit {
  public:
    ScopeExit() = default;
    ScopeExit(std::function<void()> f) : f_{std::move(f)} {}
    void operator=(std::function<void()> f) { f_ = std::move(f); }
    void reset() { f_ = {}; }
    ~ScopeExit() {
      if (f_)
        f_();
    }

  private:
    std::function<void()> f_;
  };
}
