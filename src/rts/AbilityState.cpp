#include "rts/AbilityState.h"

#include "rts/World.h"
#include "rts/constants.h"

#include <cassert>

void rts::AbilityState::trigger(World& w, Entity& e, const abilities::Instance& ai, Point target) {
  ai.trigger(w, e, activeState_, target);
  if (nextStepTime_ == GameTimeInf && activeState_)
    nextStepTime_ = w.time + 1;
}

void rts::AbilityState::step(
    const World& w, const Entity& e, AbilityStateIndex as, WorldActionList& actions) {
  assert(activeState_);
  AbilityStepResult sr{activeState_->step(w, e)};
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
    actions += [as, wid{w.weakId(e)}, f{std::move(std::get<AbilityStepAction>(sr))}](World& w) {
      if (auto* e{w[wid]})
        e->abilityStepAction(w, as, f);
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

rts::abilities::Kind rts::AbilityState::kind() const {
  return activeState_ ? activeState_->kind() : abilities::Kind::None;
}

rts::ActiveAbilityState::~ActiveAbilityState() = default;
