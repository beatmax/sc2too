#pragma once

#include "rts/Map.h"

namespace sc2 {
  class MapInitializer : public rts::MapInitializer {
  public:
    void operator()(rts::World& world, rts::Point p, char c) const final;
  };
}
