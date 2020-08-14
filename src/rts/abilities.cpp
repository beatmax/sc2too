#include "rts/abilities.h"

#include "Gather.h"
#include "Move.h"

rts::AbilityInstance rts::abilities::gather(
    AbilityId ability,
    AbilityId moveAbility,
    EntityTypeId baseType,
    GameTime gatherTime,
    GameTime deliverTime) {
  return AbilityInstance{ability,
                         state::Gather::creator(moveAbility, baseType, gatherTime, deliverTime)};
}

rts::AbilityInstance rts::abilities::move(AbilityId ability, Speed speed) {
  return AbilityInstance{ability, state::Move::creator(speed)};
}
