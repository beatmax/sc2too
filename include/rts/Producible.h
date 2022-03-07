#pragma once

#include "types.h"

#include <utility>

namespace rts {
  class Producible {
  public:
    ResourceQuantityList cost;
    GameTime produceTime;
    UiUPtr ui;

    Producible(ResourceQuantityList c, GameTime pt, UiUPtr ui)
      : cost{std::move(c)}, produceTime{pt}, ui{std::move(ui)} {}
  };
}
