#pragma once

#include "rts/types.h"

namespace ui {
  class Camera {
  public:
    constexpr static rts::Coordinate width{27};
    constexpr static rts::Coordinate height{18};

    Camera(rts::Point topLeft, rts::Point maxBottomRight)
      : area_{topLeft, {width, height}}, maxBottomRight_{maxBottomRight} {}

    const rts::Rectangle& area() const { return area_; }
    const rts::Point topLeft() const { return area_.topLeft; }
    const rts::Point bottomRight() const { return area_.topLeft + area_.size; }

    void move(const rts::Vector& v);

  private:
    rts::Rectangle area_;
    rts::Point topLeft_{0, 0};
    const rts::Point maxBottomRight_{0, 0};
  };
}
