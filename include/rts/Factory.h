#pragma once

#include "types.h"

namespace rts {
  struct Factory {
  public:
    virtual ~Factory() = default;
    virtual UnitId create(World& w, UnitTypeId t, Point p, SideId sd) = 0;
  };
}
