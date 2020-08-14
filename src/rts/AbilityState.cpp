#include "rts/AbilityState.h"

#include "rts/World.h"
#include "rts/constants.h"

#include <cassert>

void rts::AbilityState::step(
    const World& w,
    const Entity& entity,
    EntityAbilityIndex abilityIndex,
    WorldActionList& actions) {
  assert(activeState_);
  GameTime t = activeState_->step(w, entity, abilityIndex, actions);
  if (t == GameTimeInf)
    nextStepTime_ = t;
  else if (t)
    nextStepTime_ = w.time + t;
  else
    cancel(w, actions);
}

void rts::AbilityState::stepAction(World& w, Entity& entity) {
  assert(activeState_);
  GameTime t = activeState_->stepAction(w, entity);
  if (t) {
    nextStepTime_ = w.time + t;
  }
  else {
    WorldActionList actions;
    cancel(w, actions);
    w.update(actions);
  }
}

void rts::AbilityState::cancel(const World& w, WorldActionList& actions) {
  if (activeState_) {
    activeState_->cancel(w, actions);
    activeState_.reset();
  }
  nextStepTime_ = GameTimeInf;
}

rts::ActiveAbilityState::~ActiveAbilityState() = default;
