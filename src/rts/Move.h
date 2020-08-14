#pragma once

#include "rts/AbilityState.h"
#include "rts/Path.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class Move : public ActiveAbilityStateTpl<Move> {
  public:
    explicit Move(Point target, Speed speed) : target_{target}, speed_{speed} {}
    GameTime step(
        const World& w,
        const Entity& entity,
        EntityAbilityIndex abilityIndex,
        WorldActionList& actions) final;
    GameTime stepAction(World& w, Entity& entity) final;
    void cancel(const World& w, WorldActionList& actions) final {}
    int state() const final { return int(abilities::MoveState::Moving); }

  private:
    const Point target_;
    Speed speed_;
    Path path_;
    bool completePath_{false};
  };
}
