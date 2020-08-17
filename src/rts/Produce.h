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
        World& w, Entity& e, ActiveAbilityStateUPtr& as, const Desc& desc, Point target);

    AbilityStepResult step(const World& w, const Entity& e) final;
    void cancel(World& w) final {}
    int state() const final { return int(state_); }

  private:
    AbilityStepAction startProduction();
    AbilityStepAction finishProduction();

    State state_{State::Idle};
  };
}
