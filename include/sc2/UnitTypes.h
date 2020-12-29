#pragma once

#include "rts/types.h"

namespace sc2 {
  struct UnitTypes {
    static void init(rts::World& w);

    static rts::UnitTypeId gateway;
    static rts::UnitTypeId nexus;
    static rts::UnitTypeId probe;
    static rts::UnitTypeId pylon;
    static rts::UnitTypeId zealot;
  };
}
