#include "rts/WorldAction.h"

#include "rts/World.h"

void rts::addStepAction(
    const World& w,
    const Entity& entity,
    EntityAbilityIndex abilityIndex,
    WorldActionList& actions) {
  addAction(actions, action::AbilityStepAction{w.entities.weakId(entity), abilityIndex});
}
