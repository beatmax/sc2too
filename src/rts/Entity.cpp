#include "rts/Entity.h"

#include "rts/World.h"
#include "rts/WorldAction.h"

#include <algorithm>

rts::Entity::Entity(Point p, Vector s, EntityTypeId t, SideId sd, UiUPtr ui)
  : WorldObject{p, s, std::move(ui)}, type{t}, side{sd} {
}

rts::WorldActionList rts::Entity::trigger(
    AbilityId ability, const World& w, Point target) const {
  WorldActionList actions;

  const auto& entityType{w[type]};
  auto ai{entityType.abilityIndex(ability)};
  if (ai == EntityAbilityIndex::None)
    return actions;

  cancelAll();

  AbilityState& abilityState{abilityStates[ai]};
  abilityState.trigger(entityType.abilities[ai], target);

  abilityState.step(w, *this, ai, actions);
  nextStepTime = std::min(nextStepTime, abilityState.nextStepTime());
  return actions;
}

rts::WorldActionList rts::Entity::step(const World& w) const {
  WorldActionList actions;
  nextStepTime = GameTimeInf;
  for (size_t i = 0; i < MaxEntityAbilities; ++i) {
    EntityAbilityIndex ai{i};
    auto& abilityState{abilityStates[ai]};
    if (abilityState.nextStepTime() == w.time) {
      abilityState.step(w, *this, ai, actions);
      nextStepTime = std::min(nextStepTime, abilityState.nextStepTime());
    }
  }
  return actions;
}

void rts::Entity::stepAction(World& w, EntityAbilityIndex abilityIndex) {
  auto& abilityState{abilityStates[abilityIndex]};
  abilityState.stepAction(w, *this);
  nextStepTime = std::min(nextStepTime, abilityState.nextStepTime());
}

void rts::Entity::cancelAll() const {
  for (auto& abilityState : abilityStates)
    abilityState.cancel();
}
