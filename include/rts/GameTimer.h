#pragma once

#include "rts/types.h"

namespace rts {
  class GameTimer {
  public:
    explicit GameTimer(GameTime i) : interval_{i} {}
    void setInterval(GameTime i) { interval_ = i; }
    void operator++() {
      if (++elapsed_ == interval_)
        elapsed_ = 0;
    }
    bool tick() const { return !elapsed_; }

  private:
    GameTime interval_;
    GameTime elapsed_{0};
  };
}
