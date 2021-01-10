#pragma once

#include "rts/types.h"

namespace ui {
  class Camera {
  public:
    constexpr static rts::Vector Size{27, 18};

    Camera(rts::Point topLeft, const rts::Rectangle& mapArea)
      : area_{topLeft, Size}, bounds_{mapArea} {}

    const rts::Rectangle& area() const { return area_; }
    const rts::Point topLeft() const { return area_.topLeft; }
    const rts::Point bottomRight() const { return area_.bottomRight(); }
    const rts::Vector& moveDirection() const { return moveDirection_; }

    void setCenter(rts::Point p);
    void setTopLeft(rts::Point p) { area_.topLeft = p; }
    void setMoveDirection(const rts::Vector& v) { moveDirection_ = v; }
    void update();

  private:
    rts::Rectangle area_;
    const rts::Rectangle bounds_;

    rts::Vector moveDirection_{0, 0};
    constexpr static int movePerid_{2};
    int moveCnt_{movePerid_};
  };
}
