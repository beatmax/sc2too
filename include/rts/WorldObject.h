#pragma once

#include "types.h"

#include <utility>

namespace rts {
  struct WorldObject {
    Rectangle area;
    Direction direction{Direction::Right};
    UiUPtr ui;

  protected:
    WorldObject(Point p, Vector s, UiUPtr ui) : area{p, s}, ui{std::move(ui)} {}
    WorldObject(WorldObject&&) = default;

    ~WorldObject() = default;
  };
}
