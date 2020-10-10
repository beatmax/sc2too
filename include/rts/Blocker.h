#pragma once

#include "WorldObject.h"

#include <utility>

namespace rts {
  class Blocker : public WorldObject {
  public:
    Blocker(Vector s, UiUPtr ui) : WorldObject{{-1, -1}, s, std::move(ui)} {}
  };
}
