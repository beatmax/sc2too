#pragma once

#include "rts/types.h"

namespace sc2 {
  struct Abilities {
    static void init(rts::World& w);

    static rts::AbilityId gather;
    static rts::AbilityId move;
    static rts::AbilityId warpInProbe;

    static constexpr rts::AbilityInstanceIndex GatherIndex{5};
    static constexpr rts::AbilityInstanceIndex MoveIndex{0};
    static constexpr rts::AbilityInstanceIndex WarpInProbeIndex{0};
  };
}
