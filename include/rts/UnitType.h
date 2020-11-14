#pragma once

#include "abilities.h"
#include "constants.h"
#include "types.h"

#include <array>
#include <utility>

namespace rts {
  class UnitType {
  public:
    enum class Kind { Structure, Worker, Army };

    std::array<abilities::Instance, MaxUnitAbilities> abilities{};
    std::array<AbilityInstanceIndex, uint32_t(RelativeContent::Count)> defaultAbility{};
    Kind kind;
    ResourceQuantityList cost;
    ResourceQuantityList provision;
    GameTime buildTime;
    UiUPtr ui;

    UnitType(Kind k, ResourceQuantityList c, ResourceQuantityList p, GameTime bt, UiUPtr ui)
      : kind{k}, cost{std::move(c)}, provision{std::move(p)}, buildTime{bt}, ui{std::move(ui)} {}

    template<typename D>
    void addAbility(AbilityInstanceIndex index, const D& desc) {
      abilities[index] = abilities::instance(desc, abilityStateIndexFor(D::kind));
    }

    AbilityInstanceIndex abilityIndex(AbilityId ability) const;
    AbilityInstanceIndex abilityIndex(abilities::Kind kind) const;
    AbilityStateIndex abilityStateIndex(abilities::Kind kind) const;

  private:
    AbilityStateIndex abilityStateIndexFor(abilities::Kind kind) const;
  };
}
