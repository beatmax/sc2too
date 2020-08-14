#pragma once

#include "Ability.h"
#include "types.h"

namespace rts::abilities {
  enum class GatherState {
    Init,
    MovingToTarget,
    Occupying,
    Gathering,
    GatheringDone,
    MovingToBase,
    Delivering,
    DeliveringDone
  };

  enum class MoveState { Moving };

  AbilityInstance gather(
      AbilityId ability,
      AbilityId moveAbility,
      EntityTypeId baseType,
      GameTime gatherTime,
      GameTime deliverTime);
  AbilityInstance move(AbilityId ability, Speed speed);
}
