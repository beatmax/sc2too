#include "rts/Ability.h"

#include "rts/World.h"
#include "rts/dimensions.h"

#include <cassert>

void rts::Ability::step(const World& world, const Entity& entity, WorldActionList& actions) {
  assert(state_);
  GameTime t = state_->step(world, entity, *this, actions);
  if (t == GameTimeInf)
    nextStepTime_ = t;
  else if (t)
    nextStepTime_ = world.time + t;
  else
    cancel();
}

void rts::Ability::stepAction(World& world, Entity& entity) {
  assert(state_);
  GameTime t = state_->stepAction(world, entity, *this);
  if (t)
    nextStepTime_ = world.time + t;
  else
    cancel();
}

void rts::Ability::cancel() {
  state_.reset();
  nextStepTime_ = GameTimeInf;
}
