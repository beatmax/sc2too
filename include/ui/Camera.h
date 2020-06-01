#pragma once

#include "rts/types.h"

namespace ui {
  class Camera {
  public:
    constexpr static rts::Coordinate width{27};
    constexpr static rts::Coordinate height{18};

    Camera(rts::Point topLeft, rts::Point maxBottomRight)
      : topLeft_{topLeft}, maxBottomRight_{maxBottomRight} {}

    const rts::Point topLeft() const { return topLeft_; }
    const rts::Point bottomRight() const { return topLeft_ + rts::Vector{width, height}; }

    void move(const rts::Vector& v);

  private:
    rts::Point topLeft_{0, 0};
    const rts::Point maxBottomRight_{0, 0};
  };
}
