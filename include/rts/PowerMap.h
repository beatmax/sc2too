#pragma once

#include "ProximityMap.h"

namespace rts {

  class PowerMap : public ProximityMap {
  public:
    void update(World& w, SideId side, const Circle& c, int16_t inc);
  };
}
