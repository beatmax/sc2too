#include "rts/AbilityState.h"

#include "rts/World.h"
#include "rts/constants.h"

#include <cassert>

void rts::AbilityState::trigger(World& w, const AbilityInstance& ability, Point target) {
  activeState_ = ability.createActiveState(target);
  nextStepTime_ = w.time + 1;
}

void rts::AbilityState::step(
    const World& w,
    const Entity& entity,
    EntityAbilityIndex abilityIndex,
    WorldActionList& actions) {
  assert(activeState_);
  AbilityStepResult sr{activeState_->step(w, entity, abilityIndex)};
  if (auto* t{std::get_if<GameTime>(&sr)}) {
    if (*t == GameTimeInf)
      nextStepTime_ = *t;
    else if (*t)
      nextStepTime_ = w.time + *t;
    else {
      activeState_.reset();
      nextStepTime_ = GameTimeInf;
    }
  }
  else {
    actions += [abilityIndex, wid{w.weakId(entity)},
                f{std::move(std::get<AbilityStepAction>(sr))}](World& w) {
      if (auto* e{w[wid]})
        e->stepAction(w, abilityIndex, f);
    };
    nextStepTime_ = GameTimeInf;
  }
}

void rts::AbilityState::stepAction(World& w, Entity& e, const AbilityStepAction& f) {
  if (!activeState_)
    return;

  GameTime t{f(w, e)};
  if (t) {
    nextStepTime_ = w.time + t;
  }
  else {
    activeState_.reset();
    nextStepTime_ = GameTimeInf;
  }
}

void rts::AbilityState::cancel(World& w) {
  if (activeState_) {
    activeState_->cancel(w);
    activeState_.reset();
  }
  nextStepTime_ = GameTimeInf;
}

rts::ActiveAbilityState::~ActiveAbilityState() = default;
