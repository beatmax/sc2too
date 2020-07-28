#pragma once

#include "Ability.h"
#include "WorldObject.h"
#include "dimensions.h"
#include "types.h"
#include "util/Pool.h"

#include <utility>

namespace rts {
  class Entity : public WorldObject {
  public:
    constexpr static size_t MaxAbilities{15};
    using AbilityPool = util::Pool<Ability, MaxAbilities>;

    SideId side;
    mutable AbilityPool abilities;
    mutable GameTime nextStepTime{GameTimeInf};

    Entity(Point p, Vector s, SideId sd, UiUPtr ui) : WorldObject{p, s, std::move(ui)}, side{sd} {}

    AbilityId addAbility(Ability&& a) { return abilities.emplace(std::move(a)); }

    WorldActionList trigger(AbilityId a, const World& world, Point target) const;
    WorldActionList step(const World& world) const;
    void stepAction(World& world, AbilityId a);
    void cancelAll() const;
  };
}
