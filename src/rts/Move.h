#pragma once

#include "rts/AbilityState.h"
#include "rts/Path.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class Move : public ActiveAbilityStateTpl<Move> {
  public:
    using Desc = abilities::Move;
    using State = abilities::MoveState;

    static void trigger(
        World& w,
        Unit& u,
        ActiveAbilityStateUPtr& as,
        const Desc& desc,
        const AbilityTarget& target);

    explicit Move(const Desc& desc, const AbilityTarget& target) : desc_{desc}, target_{target} {}
    AbilityStepResult step(const World& w, UnitStableRef u);
    void cancel(World& w) final {}
    int state() const final { return int(State::Moving); }

  private:
    AbilityStepAction stepAction();

    const Desc desc_;
    const AbilityTarget target_;
    Path path_;
    bool completePath_{false};
  };
}
