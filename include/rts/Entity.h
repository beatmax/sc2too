#pragma once

#include "AbilityState.h"
#include "Resource.h"
#include "WorldObject.h"
#include "constants.h"
#include "types.h"
#include "util/Pool.h"

#include <array>
#include <optional>
#include <utility>

namespace rts {
  class Entity : public WorldObject {
  public:
    EntityTypeId type;
    SideId side;
    ResourceBag bag;
    mutable std::array<AbilityState, MaxEntityAbilities> abilityStates;
    mutable GameTime nextStepTime{GameTimeInf};

    Entity(Point p, Vector s, EntityTypeId t, SideId sd, Quantity cargoCapacity, UiUPtr ui);
    WorldActionList onDestroy(const World& w) const __attribute__((warn_unused_result));

    WorldActionList trigger(AbilityId ability, const World& w, Point target) const
        __attribute__((warn_unused_result));
    WorldActionList step(const World& w) const __attribute__((warn_unused_result));
    void triggerNested(
        AbilityId ability, const World& w, Point target, WorldActionList& actions) const;
    void stepAction(World& w, EntityAbilityIndex abilityIndex);
    void cancelAll(const World& w, WorldActionList& actions) const;

    template<typename T>
    std::optional<T> state(EntityAbilityIndex abilityIndex) const {
      return abilityStates[abilityIndex].state<T>();
    }
  };
}
