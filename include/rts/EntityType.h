#pragma once

#include "Ability.h"
#include "constants.h"
#include "types.h"

#include <algorithm>
#include <array>
#include <utility>

namespace rts {
  class EntityType {
  public:
    std::array<AbilityInstance, MaxEntityAbilities> abilities{};
    AbilityId defaultAbilityOnFriend{};
    AbilityId defaultAbilityOnFoe{};
    AbilityId defaultAbilityOnGround{};
    UiUPtr ui;

    explicit EntityType(UiUPtr ui) : ui{std::move(ui)} {}

    EntityAbilityIndex abilityIndex(AbilityId ability) const {
      auto it = std::find_if(abilities.begin(), abilities.end(), [ability](const auto& ai) {
        return ai.abilityId == ability;
      });
      return (it != abilities.end()) ? EntityAbilityIndex(it - abilities.begin())
                                     : EntityAbilityIndex::None;
    }
  };
}
