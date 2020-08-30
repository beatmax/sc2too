#include "rts/Unit.h"

#include "rts/World.h"
#include "rts/WorldAction.h"

#include <algorithm>
#include <cassert>

rts::Unit::Unit(
    Point p,
    Vector s,
    UnitTypeId t,
    SideId sd,
    UiUPtr ui,
    Quantity cargoCapacity,
    ProductionQueueId pq)
  : WorldObject{p, s, std::move(ui)},
    type{t},
    side{sd},
    bag{{}, 0, cargoCapacity},
    productionQueue{pq} {
}

void rts::Unit::onDestroy(World& w) {
  cancelAll(w);
  if (productionQueue)
    w.destroy(productionQueue);

  const auto& t{w[type]};
  auto& res{w[side].resources()};
  res.deallocate(t.cost);
  res.deprovision(t.provision);
}

void rts::Unit::trigger(AbilityId ability, World& w, Point target, CancelOthers cancelOthers) {
  const auto& unitType{w[type]};
  auto ai{unitType.abilityIndex(ability)};
  if (ai == AbilityInstanceIndex::None)
    return;

  const auto& abilityInstance{unitType.abilities[ai]};
  AbilityState& abilityState{abilityStates_[abilityInstance.stateIndex]};

  if (cancelOthers == CancelOthers::Yes) {
    for (auto& as : abilityStates_) {
      if (&as != &abilityState && as.active())
        as.cancel(w);
    }
  }

  abilityState.trigger(w, *this, abilityInstance, target);
  nextStepTime_ = std::min(nextStepTime_, abilityState.nextStepTime());
}

rts::WorldActionList rts::Unit::step(UnitStableRef u, const World& w) {
  WorldActionList actions;
  if (u->nextStepTime_ != w.time) {
    assert(u->nextStepTime_ > w.time);
    return actions;
  }

  u->nextStepTime_ = GameTimeInf;
  for (size_t i = 0; i < MaxUnitAbilities; ++i) {
    AbilityStateIndex as{i};
    auto& abilityState{u->abilityStates_[as]};
    if (abilityState.nextStepTime() == w.time)
      abilityState.step(w, u, as, actions);
    u->nextStepTime_ = std::min(u->nextStepTime_, abilityState.nextStepTime());
  }
  return actions;
}

void rts::Unit::abilityStepAction(World& w, AbilityStateIndex as, const AbilityStepAction& f) {
  auto& abilityState{abilityStates_[as]};
  abilityState.stepAction(w, *this, f);
  nextStepTime_ = std::min(nextStepTime_, abilityState.nextStepTime());
}

void rts::Unit::cancelAll(World& w) {
  for (auto& as : abilityStates_) {
    if (as.active())
      as.cancel(w);
  }
}

const rts::AbilityState& rts::Unit::abilityState(
    UnitStableRef u, const World& w, abilities::Kind kind) {
  auto& t{w[u->type]};
  auto as{t.abilityStateIndex(kind)};
  assert(as != AbilityStateIndex::None);
  return u->abilityStates_[as];
}
