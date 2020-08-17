#pragma once

#include "abilities.h"
#include "constants.h"
#include "types.h"

#include <array>
#include <utility>

namespace rts {
  class EntityType {
  public:
    std::array<abilities::Instance, MaxEntityAbilities> abilities{};
    std::array<AbilityId, size_t(RelativeContent::Count)> defaultAbility{};
    ResourceQuantityList cost;
    GameTime buildTime;
    UiUPtr ui;

    EntityType(ResourceQuantityList c, GameTime bt, UiUPtr ui)
      : cost{std::move(c)}, buildTime{bt}, ui{std::move(ui)} {}

    template<typename D>
    void addAbility(AbilityInstanceIndex index, const D& desc) {
      abilities[index] = abilities::instance(desc, abilityStateIndexFor(D::kind));
    }

    AbilityInstanceIndex abilityIndex(AbilityId ability) const;
    AbilityStateIndex abilityStateIndex(abilities::Kind kind) const;

  private:
    AbilityStateIndex abilityStateIndexFor(abilities::Kind kind) const;
  };
}
