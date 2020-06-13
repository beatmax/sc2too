#pragma once

#include <chrono>

namespace rts {
  class Engine {
  public:
    explicit Engine(int targetFps) : targetFps_{targetFps} {}
    void advanceFrame();

  private:
    using timePoint = std::chrono::steady_clock::time_point;

    const int targetFps_;
    timePoint nextFrameTime_;
  };
}
