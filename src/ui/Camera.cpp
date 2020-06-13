#include "ui/Camera.h"

void ui::Camera::update() {
  if (--moveCnt_ > 0) {
    return;
  }
  moveCnt_ = movePerid_;

  area_.topLeft += moveDirection_;
  if (area_.topLeft.x < 0)
    area_.topLeft.x = 0;
  if (area_.topLeft.y < 0)
    area_.topLeft.y = 0;
  const auto& br{bottomRight()};
  if (br.x >= maxBottomRight_.x)
    area_.topLeft.x = maxBottomRight_.x - area_.size.x;
  if (br.y >= maxBottomRight_.y)
    area_.topLeft.y = maxBottomRight_.y - area_.size.y;
}
