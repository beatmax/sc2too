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
    enum class CancelOthers { No, Yes };

    EntityTypeId type;
    SideId side;
    ResourceBag bag;

    mutable std::array<AbilityState, MaxEntityAbilities> abilityStates;
    mutable GameTime nextStepTime{GameTimeInf};

    Entity(Point p, Vector s, EntityTypeId t, SideId sd, Quantity cargoCapacity, UiUPtr ui);
    void onDestroy(World& w);

    void trigger(
        AbilityId ability, World& w, Point target, CancelOthers cancelOthers = CancelOthers::Yes);
    WorldActionList step(const World& w) const __attribute__((warn_unused_result));
    void stepAction(World& w, EntityAbilityIndex abilityIndex, const AbilityStepAction& f);
    void cancelAll(World& w);

    template<typename T>
    std::optional<T> state(EntityAbilityIndex abilityIndex) const {
      return abilityStates[abilityIndex].state<T>();
    }
  };
}
