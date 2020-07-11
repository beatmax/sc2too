#include "rts/Entity.h"

#include "rts/World.h"
#include "rts/WorldAction.h"

#include <algorithm>

rts::WorldActionList rts::Entity::trigger(AbilityId a, const World& world, Point target) const {
  Ability& ability{abilities[a]};
  cancelAll();
  ability.trigger(target);

  WorldActionList actions;
  ability.step(world, *this, actions);
  nextStepTime = std::min(nextStepTime, ability.nextStepTime());
  return actions;
}

rts::WorldActionList rts::Entity::step(const World& world) const {
  WorldActionList actions;
  nextStepTime = GameTimeInf;
  for (auto& a : abilities) {
    if (a.nextStepTime() == world.time) {
      a.step(world, *this, actions);
      nextStepTime = std::min(nextStepTime, a.nextStepTime());
    }
  }
  return actions;
}

void rts::Entity::stepAction(World& world, AbilityId a) {
  auto& ability{abilities[a]};
  ability.stepAction(world, *this);
  nextStepTime = std::min(nextStepTime, ability.nextStepTime());
}

void rts::Entity::cancelAll() const {
  for (auto& a : abilities)
    a.cancel();
}
