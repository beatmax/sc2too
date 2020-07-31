#include "rts/AbilityState.h"

#include "rts/World.h"
#include "rts/dimensions.h"

#include <cassert>

void rts::AbilityState::step(
    const World& world,
    const Entity& entity,
    EntityAbilityIndex abilityIndex,
    WorldActionList& actions) {
  assert(activeState_);
  GameTime t = activeState_->step(world, entity, abilityIndex, actions);
  if (t == GameTimeInf)
    nextStepTime_ = t;
  else if (t)
    nextStepTime_ = world.time + t;
  else
    cancel();
}

void rts::AbilityState::stepAction(World& world, Entity& entity) {
  assert(activeState_);
  GameTime t = activeState_->stepAction(world, entity);
  if (t)
    nextStepTime_ = world.time + t;
  else
    cancel();
}

void rts::AbilityState::cancel() {
  activeState_.reset();
  nextStepTime_ = GameTimeInf;
}

rts::ActiveAbilityState::~ActiveAbilityState() = default;
