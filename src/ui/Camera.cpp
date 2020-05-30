#include "ui/Camera.h"

void ui::Camera::move(const rts::Vector& v) {
  topLeft_ += v;
  if (topLeft_.x < 0)
    topLeft_.x = 0;
  if (topLeft_.y < 0)
    topLeft_.y = 0;
  const auto& br{bottomRight()};
  if (br.x >= maxBottomRight_.x)
    topLeft_.x = maxBottomRight_.x - width;
  if (br.y >= maxBottomRight_.y)
    topLeft_.y = maxBottomRight_.y - height;
}
