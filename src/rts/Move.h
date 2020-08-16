#pragma once

#include "rts/AbilityState.h"
#include "rts/Path.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class Move : public ActiveAbilityStateTpl<Move> {
  public:
    explicit Move(Point target, Speed speed) : target_{target}, speed_{speed} {}
    AbilityStepResult step(
        const World& w, const Entity& entity, EntityAbilityIndex abilityIndex) final;
    void cancel(World& w) final {}
    int state() const final { return int(abilities::MoveState::Moving); }

  private:
    AbilityStepAction stepAction(const World& w);

    const Point target_;
    Speed speed_;
    Path path_;
    bool completePath_{false};
  };
}
