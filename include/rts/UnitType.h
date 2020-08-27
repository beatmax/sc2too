#pragma once

#include "abilities.h"
#include "constants.h"
#include "types.h"

#include <array>
#include <utility>

namespace rts {
  class UnitType {
  public:
    std::array<abilities::Instance, MaxUnitAbilities> abilities{};
    std::array<AbilityId, size_t(RelativeContent::Count)> defaultAbility{};
    ResourceQuantityList cost;
    ResourceQuantityList provision;
    GameTime buildTime;
    UiUPtr ui;

    UnitType(ResourceQuantityList c, ResourceQuantityList p, GameTime bt, UiUPtr ui)
      : cost{std::move(c)}, provision{std::move(p)}, buildTime{bt}, ui{std::move(ui)} {}

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
