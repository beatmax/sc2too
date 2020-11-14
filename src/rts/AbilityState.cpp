#include "rts/AbilityState.h"

#include "rts/World.h"
#include "rts/constants.h"

#include <cassert>

void rts::AbilityState::trigger(
    World& w,
    Unit& u,
    TriggerGroup& group,
    const abilities::Instance& ai,
    const AbilityTarget& target) {
  auto [newState, t] = ai.trigger(w, u, group, activeState_, target);
  if (newState) {
    if (activeState_)
      activeState_->cancel(w);
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
      activeState_.reset();
      nextStepTime_ = GameTimeInf;
      actions += [as, wid{w.weakId(*u)}](World& w) {
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

  GameTime t{f(w, u)};
  if (t) {
    nextStepTime_ = w.time + t;
  }
  else {
    destroyActiveState(u, as);
    nextStepTime_ = GameTimeInf;
  }
}

void rts::AbilityState::cancel(World& w, Unit& u, AbilityStateIndex as) {
  if (activeState_) {
    activeState_->cancel(w);
    destroyActiveState(u, as);
  }
  nextStepTime_ = GameTimeInf;
}

void rts::AbilityState::destroyActiveState(Unit& u, AbilityStateIndex as) {
  activeState_.reset();
  u.abilityActive[as] = false;
}

rts::ActiveAbilityState::~ActiveAbilityState() = default;
