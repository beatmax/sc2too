#pragma once

#include "types.h"

namespace rts {
  class Factory {
  public:
    virtual ~Factory() = default;
    virtual void init(World& w) = 0;
    virtual UnitId create(World& w, UnitTypeId t, SideId sd) = 0;
  };
}
