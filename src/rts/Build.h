#pragma once

#include "rts/AbilityState.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class Build : public ActiveAbilityBuildStateTpl<Build> {
  public:
    using Desc = abilities::Build;
    using State = abilities::BuildState;

    static ActiveAbilityStateUPtr trigger(
        World& w, Unit& u, ActiveAbilityStateUPtr& as, const Desc& desc, UnitId prototype);

    explicit Build(const Desc& desc, UnitId builtUnit) : desc_{desc}, builtUnit_{builtUnit} {}

    AbilityStepResult step(const World& w, UnitStableRef u) final;
    void cancel(World& w) final;
    int state() const final { return int(state_); }

  private:
    AbilityStepResult init(const World& w, const Unit& unit);
    AbilityStepAction build();

    const Desc desc_;
    State state_{State::Init};
    UnitId builtUnit_;
    int retryCount_{0};
  };
}
