#pragma once

#include "types.h"

namespace rts {
  struct Factory {
  public:
    virtual ~Factory() = default;
    virtual EntityId create(World& w, EntityTypeId t, Point p, SideId sd) = 0;
  };
}
