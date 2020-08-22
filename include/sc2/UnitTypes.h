#pragma once

#include "rts/types.h"

namespace sc2 {
  struct UnitTypes {
    static void init(rts::World& w);

    static rts::UnitTypeId nexus;
    static rts::UnitTypeId probe;
  };
}
