#pragma once

#include "rts/types.h"

namespace ui {
  class Camera {
  public:
    constexpr static rts::Coordinate width{27};
    constexpr static rts::Coordinate height{18};

    Camera(rts::Point topLeft, const rts::Rectangle& mapArea)
      : area_{topLeft, {width, height}}, bottomRightOut_{mapArea.bottomRightOut()} {}

    const rts::Rectangle& area() const { return area_; }
    const rts::Point topLeft() const { return area_.topLeft; }
    const rts::Point bottomRight() const { return area_.bottomRight(); }

    const rts::Vector& moveDirection() const { return moveDirection_; }
    void setMoveDirection(const rts::Vector& v) { moveDirection_ = v; }
    void update();

  private:
    rts::Rectangle area_;
    const rts::Point bottomRightOut_;

    rts::Vector moveDirection_{0, 0};
    constexpr static int movePerid_{2};
    int moveCnt_{movePerid_};
  };
}
