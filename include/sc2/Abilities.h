#pragma once

#include "rts/types.h"

namespace sc2 {
  struct Abilities {
    static void init(rts::World& w);

    static rts::AbilityId gather;
    static rts::AbilityId move;

    static constexpr rts::EntityAbilityIndex GatherIndex{5};
    static constexpr rts::EntityAbilityIndex MoveIndex{0};
  };
}
