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
    enum class RequiresPower { No, Yes };

    std::array<abilities::Instance, MaxUnitAbilities> abilities{};
    std::array<AbilityInstanceIndex, uint32_t(RelativeContent::Count)> defaultAbility{};
    Kind kind;
    ResourceQuantityList cost;
    ResourceQuantityList provision;
    GameTime buildTime;
    UiUPtr ui;
    Quantity maxEnergy;
    Quantity initialEnergy;
    RequiresPower requiresPower;
    Coordinate powerRadius;
    ResourceId extractedResource;

    UnitType(
        Kind k,
        ResourceQuantityList c,
        ResourceQuantityList p,
        GameTime bt,
        UiUPtr ui,
        Quantity me = 0,
        Quantity ie = 0,
        RequiresPower reqp = RequiresPower::No,
        Coordinate pr = 0,
        ResourceId er = {})
      : kind{k},
        cost{std::move(c)},
        provision{std::move(p)},
        buildTime{bt},
        ui{std::move(ui)},
        maxEnergy{me},
        initialEnergy{ie},
        requiresPower{reqp},
        powerRadius{pr},
        extractedResource{er} {}

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
