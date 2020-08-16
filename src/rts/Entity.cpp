#include "rts/Entity.h"

#include "rts/World.h"
#include "rts/WorldAction.h"

#include <algorithm>
#include <cassert>

rts::Entity::Entity(Point p, Vector s, EntityTypeId t, SideId sd, Quantity cargoCapacity, UiUPtr ui)
  : WorldObject{p, s, std::move(ui)}, type{t}, side{sd}, bag{nullptr, 0, cargoCapacity} {
}

void rts::Entity::onDestroy(World& w) {
  cancelAll(w);
}

void rts::Entity::trigger(AbilityId ability, World& w, Point target, CancelOthers cancelOthers) {
  const auto& entityType{w[type]};
  auto ai{entityType.abilityIndex(ability)};
  if (ai == EntityAbilityIndex::None)
    return;

  if (cancelOthers == CancelOthers::Yes)
    cancelAll(w);

  AbilityState& abilityState{abilityStates[ai]};
  abilityState.trigger(w, entityType.abilities[ai], target);
  nextStepTime = w.time + 1;
}

rts::WorldActionList rts::Entity::step(const World& w) const {
  WorldActionList actions;
  if (nextStepTime != w.time) {
    assert(nextStepTime > w.time);
    return actions;
  }

  nextStepTime = GameTimeInf;
  for (size_t i = 0; i < MaxEntityAbilities; ++i) {
    EntityAbilityIndex ai{i};
    auto& abilityState{abilityStates[ai]};
    if (abilityState.nextStepTime() == w.time)
      abilityState.step(w, *this, ai, actions);
    nextStepTime = std::min(nextStepTime, abilityState.nextStepTime());
  }
  return actions;
}

void rts::Entity::stepAction(
    World& w, EntityAbilityIndex abilityIndex, const AbilityStepAction& f) {
  auto& abilityState{abilityStates[abilityIndex]};
  abilityState.stepAction(w, *this, f);
  nextStepTime = std::min(nextStepTime, abilityState.nextStepTime());
}

void rts::Entity::cancelAll(World& w) {
  for (auto& abilityState : abilityStates)
    abilityState.cancel(w);
}
