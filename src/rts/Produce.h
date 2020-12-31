#pragma once

#include "rts/AbilityState.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class Produce : public ActiveAbilityStateTpl<Produce> {
  public:
    using Desc = abilities::Produce;
    using State = abilities::ProduceState;

    static ActiveAbilityStateUPtr trigger(
        World& w,
        Unit& u,
        ActiveAbilityStateUPtr& as,
        const Desc& desc,
        const AbilityTarget& target);

    AbilityStepResult step(const World& w, UnitStableRef u) final;
    void cleanup(World& w) final {}
    int state() const final { return int(state_); }

    void unblock();

  private:
    AbilityStepAction startProduction();
    AbilityStepAction finishProduction();

    State state_{State::Idle};
  };
}
