#pragma once

#include "AbilityState.h"
#include "WorldObject.h"
#include "constants.h"
#include "dimensions.h"
#include "types.h"
#include "util/Pool.h"

#include <array>
#include <utility>

namespace rts {
  class Entity : public WorldObject {
  public:
    EntityTypeId type;
    SideId side;
    mutable std::array<AbilityState, MaxEntityAbilities> abilityStates;
    mutable GameTime nextStepTime{GameTimeInf};

    Entity(Point p, Vector s, EntityTypeId t, SideId sd, UiUPtr ui);

    WorldActionList trigger(AbilityId ability, const World& world, Point target) const;
    WorldActionList step(const World& world) const;
    void stepAction(World& world, EntityAbilityIndex abilityIndex);
    void cancelAll() const;
  };
}
