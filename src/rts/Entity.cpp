#include "rts/Entity.h"

#include "rts/World.h"
#include "rts/WorldAction.h"

#include <algorithm>
#include <cassert>

rts::Entity::Entity(
    Point p,
    Vector s,
    EntityTypeId t,
    SideId sd,
    UiUPtr ui,
    Quantity cargoCapacity,
    ProductionQueueId pq)
  : WorldObject{p, s, std::move(ui)},
    type{t},
    side{sd},
    bag{nullptr, 0, cargoCapacity},
    productionQueue{pq} {
}

void rts::Entity::onDestroy(World& w) {
  cancelAll(w);
  if (productionQueue)
    w.destroy(productionQueue);
}

void rts::Entity::trigger(AbilityId ability, World& w, Point target, CancelOthers cancelOthers) {
  const auto& entityType{w[type]};
  auto ai{entityType.abilityIndex(ability)};
  if (ai == AbilityInstanceIndex::None)
    return;

  const auto& abilityInstance{entityType.abilities[ai]};
  AbilityState& abilityState{abilityStates[abilityInstance.stateIndex]};

  if (cancelOthers == CancelOthers::Yes) {
    for (auto& as : abilityStates) {
      if (&as != &abilityState && as.active())
        as.cancel(w);
    }
  }

  abilityState.trigger(w, *this, abilityInstance, target);
  nextStepTime = std::min(nextStepTime, abilityState.nextStepTime());
}

rts::WorldActionList rts::Entity::step(const World& w) const {
  WorldActionList actions;
  if (nextStepTime != w.time) {
    assert(nextStepTime > w.time);
    return actions;
  }

  nextStepTime = GameTimeInf;
  for (size_t i = 0; i < MaxEntityAbilities; ++i) {
    AbilityStateIndex as{i};
    auto& abilityState{abilityStates[as]};
    if (abilityState.nextStepTime() == w.time)
      abilityState.step(w, *this, as, actions);
    nextStepTime = std::min(nextStepTime, abilityState.nextStepTime());
  }
  return actions;
}

void rts::Entity::abilityStepAction(World& w, AbilityStateIndex as, const AbilityStepAction& f) {
  auto& abilityState{abilityStates[as]};
  abilityState.stepAction(w, *this, f);
  nextStepTime = std::min(nextStepTime, abilityState.nextStepTime());
}

void rts::Entity::cancelAll(World& w) {
  for (auto& as : abilityStates) {
    if (as.active())
      as.cancel(w);
  }
}

const rts::AbilityState& rts::Entity::abilityState(const World& w, abilities::Kind kind) const {
  auto& t{w[type]};
  auto as{t.abilityStateIndex(kind)};
  assert(as != AbilityStateIndex::None);
  return abilityStates[as];
}

const rts::AbilityStateIndex rts::Entity::activeAbilityStateIndex(abilities::Kind kind) const {
  auto it = std::find_if(
      abilityStates.begin(), abilityStates.end(),
      [kind](const AbilityState& as) { return as.kind() == kind; });
  return (it != abilityStates.end()) ? AbilityStateIndex(it - abilityStates.begin())
                                     : AbilityStateIndex::None;
}
