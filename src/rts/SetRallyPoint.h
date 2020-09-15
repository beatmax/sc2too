#pragma once

#include "rts/AbilityState.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class SetRallyPoint : public StatelessAbilityStateTpl<SetRallyPoint> {
  public:
    using Desc = abilities::SetRallyPoint;

    static void trigger(World& w, Unit& u, const Desc& desc, Point target);
  };
}
