#pragma once

#include "rts/AbilityState.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class Boost : public StatelessAbilityStateTpl<Boost> {
  public:
    using Desc = abilities::Boost;

    static void trigger(World& w, Unit& u, const Desc& desc, const AbilityTarget& target);
  };
}
