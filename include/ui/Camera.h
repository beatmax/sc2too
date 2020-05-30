#pragma once

#include "rts/types.h"

namespace ui {
  class Camera {
  public:
    constexpr static rts::Coordinate width{27};
    constexpr static rts::Coordinate height{18};

    Camera(rts::Position topLeft, rts::Position maxBottomRight)
      : topLeft_{topLeft}, maxBottomRight_{maxBottomRight} {}

    const rts::Position topLeft() const { return topLeft_; }
    const rts::Position bottomRight() const { return topLeft_ + rts::Vector{width, height}; }

    void move(const rts::Vector& v);

  private:
    rts::Position topLeft_{0, 0};
    const rts::Position maxBottomRight_{0, 0};
  };
}
