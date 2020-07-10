#pragma once

#include "Ability.h"
#include "WorldObject.h"
#include "dimensions.h"
#include "types.h"

#include <utility>

namespace rts {
  class Entity : public WorldObject {
  public:
    SideCPtr side;
    mutable AbilityList abilities;
    mutable GameTime nextStepTime{GameTimeInf};

    Entity(Point p, Vector s, SideCPtr sd, UiUPtr ui)
      : WorldObject{p, s, std::move(ui)}, side{sd} {}

    void addAbility(Ability&& a) { abilities.push_back(std::move(a)); }

    WorldActionList trigger(Ability& a, const World& world, Point target) const;
    WorldActionList step(const World& world) const;
    void cancelAll() const;
  };
}
