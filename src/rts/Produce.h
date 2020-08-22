#pragma once

#include "rts/AbilityState.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class Produce : public ActiveAbilityStateTpl<Produce> {
  public:
    using Desc = abilities::Produce;
    using State = abilities::ProduceState;

    static void trigger(
        World& w, Unit& u, ActiveAbilityStateUPtr& as, const Desc& desc, Point target);

    AbilityStepResult step(const World& w, const Unit& u) final;
    void cancel(World& w) final {}
    int state() const final { return int(state_); }

  private:
    AbilityStepAction startProduction();
    AbilityStepAction finishProduction();

    State state_{State::Idle};
  };
}
