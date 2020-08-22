#include "rts/AbilityState.h"

#include "rts/World.h"
#include "rts/constants.h"

#include <cassert>

void rts::AbilityState::trigger(World& w, Unit& u, const abilities::Instance& ai, Point target) {
  ai.trigger(w, u, activeState_, target);
  if (nextStepTime_ == GameTimeInf && activeState_)
    nextStepTime_ = w.time + 1;
}

void rts::AbilityState::step(
    const World& w, const UnitStableRef u, AbilityStateIndex as, WorldActionList& actions) {
  assert(activeState_);
  AbilityStepResult sr{activeState_->step(w, u)};
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
    actions += [as, wid{w.weakId(*u)}, f{std::move(std::get<AbilityStepAction>(sr))}](World& w) {
      if (auto* u{w[wid]})
        u->abilityStepAction(w, as, f);
    };
    nextStepTime_ = GameTimeInf;
  }
}

void rts::AbilityState::stepAction(World& w, Unit& u, const AbilityStepAction& f) {
  if (!activeState_)
    return;

  GameTime t{f(w, u)};
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
