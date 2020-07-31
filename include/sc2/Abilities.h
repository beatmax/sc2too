#pragma once

#include "rts/types.h"

namespace sc2 {
  struct Abilities {
    static void init(rts::World& w);

    static rts::AbilityId move;
  };
}
