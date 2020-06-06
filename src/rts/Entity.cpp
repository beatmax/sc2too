#include "rts/Entity.h"

#include "rts/World.h"
#include "rts/WorldAction.h"

rts::WorldActionList rts::Entity::trigger(
    Ability& a, const World& world, const EntitySPtr& entity, Point target) {
  entity->cancelAll();
  a.trigger(target);

  WorldActionList actions;
  a.step(world, entity, actions);
  return actions;
};

rts::WorldActionList rts::Entity::step(const World& world, const EntitySPtr& entity) {
  WorldActionList actions;
  for (auto& a : entity->abilities) {
    if (a.nextStepTime() == world.time)
      a.step(world, entity, actions);
  }
  return actions;
}

void rts::Entity::cancelAll() {
  for (auto& a : abilities)
    a.cancel();
}
