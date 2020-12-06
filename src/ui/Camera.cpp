#include "ui/Camera.h"

#include "util/geo.h"

void ui::Camera::setCenter(rts::Point p) {
  area_ = rectangleCenteredAt(p, area_.size, bounds_);
}

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
  if (br.x >= bounds_.size.x)
    area_.topLeft.x = bounds_.size.x - area_.size.x;
  if (br.y >= bounds_.size.y)
    area_.topLeft.y = bounds_.size.y - area_.size.y;
}
