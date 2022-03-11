#pragma once

#include "Producible.h"
#include "abilities.h"
#include "constants.h"
#include "types.h"

#include <array>
#include <utility>

namespace rts {
  class UnitType : public Producible {
  public:
    enum class Kind { Structure, Worker, Army };
    enum class RequiresPower { No, Yes };

    std::array<abilities::Instance, MaxUnitAbilities> abilities{};
    std::array<AbilityInstanceIndex, uint32_t(RelativeContent::Count)> defaultAbility{};
    Kind kind;
    ResourceQuantityList provision;
    Quantity maxEnergy;
    Quantity initialEnergy;
    UnitTypeId requiredUnit;
    RequiresPower requiresPower;
    Coordinate powerRadius;
    ResourceId extractedResource;

    UnitType(
        Kind k,
        ResourceQuantityList c,
        ResourceQuantityList p,
        GameTime pt,
        UiUPtr ui,
        Quantity me = 0,
        Quantity ie = 0,
        UnitTypeId requ = {},
        RequiresPower reqp = RequiresPower::No,
        Coordinate pr = 0,
        ResourceId er = {})
      : Producible{std::move(c), pt, std::move(ui)},
        kind{k},
        provision{std::move(p)},
        maxEnergy{me},
        initialEnergy{ie},
        requiredUnit{requ},
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
