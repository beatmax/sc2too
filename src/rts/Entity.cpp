#include "rts/Entity.h"

#include "rts/World.h"
#include "rts/WorldAction.h"

#include <algorithm>

rts::WorldActionList rts::Entity::trigger(Ability& a, const World& world, Point target) const {
  cancelAll();
  a.trigger(target);

  WorldActionList actions;
  a.step(world, *this, actions);
  nextStepTime = std::min(nextStepTime, a.nextStepTime());
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

void rts::Entity::cancelAll() const {
  for (auto& a : abilities)
    a.cancel();
}
