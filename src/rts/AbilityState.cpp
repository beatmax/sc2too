#include "rts/AbilityState.h"

#include "rts/World.h"
#include "rts/constants.h"

#include <cassert>

void rts::AbilityState::trigger(
    World& w,
    Unit& u,
    TriggerGroup& group,
    const abilities::Instance& ai,
    const AbilityTarget& target,
    UnitId prototype) {
  auto [newState, t] = ai.trigger(w, u, group, activeState_, target, prototype);
  if (newState) {
    if (activeState_)
      activeState_->cleanup(w);
    activeState_ = std::move(newState);
    nextStepTime_ = w.time + t;
    u.abilityActive[ai.stateIndex] = true;
  }
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
      nextStepTime_ = GameTimeInf;
      actions += [as, activeState{activeState_.release()}, wid{w.weakId(*u)}](World& w) {
        activeState->cleanup(w);
        delete activeState;
        if (auto* u{w[wid]})
          u->abilityActive[as] = false;
      };
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

void rts::AbilityState::stepAction(
    World& w, Unit& u, AbilityStateIndex as, const AbilityStepAction& f) {
  if (!activeState_)
    return;

  if (GameTime t{f(w, u)}; t)
    nextStepTime_ = w.time + t;
  else
    cancel(w, u, as);
}

void rts::AbilityState::cancel(World& w, Unit& u, AbilityStateIndex as) {
  if (activeState_) {
    activeState_->cleanup(w);
    activeState_.reset();
    u.abilityActive[as] = false;
  }
  nextStepTime_ = GameTimeInf;
}

rts::ActiveAbilityState::~ActiveAbilityState() = default;
