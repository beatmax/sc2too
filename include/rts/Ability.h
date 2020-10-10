#pragma once

#include "types.h"

#include <utility>

namespace rts {
  class Ability {
  public:
    UiUPtr ui;

    Ability(UiUPtr ui) : ui{std::move(ui)} {}
  };
}
