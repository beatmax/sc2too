#pragma once

#include "rts/types.h"

namespace sc2 {
  struct EntityTypes {
    static void init(rts::World& w);

    static rts::EntityTypeId nexus;
    static rts::EntityTypeId probe;
  };
}
