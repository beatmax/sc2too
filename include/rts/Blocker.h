#pragma once

#include "WorldObject.h"

#include <utility>

namespace rts {
  class Blocker : public WorldObject {
  public:
    Blocker(Point p, Vector s, UiUPtr ui) : WorldObject{p, s, std::move(ui)} {}
  };
}
