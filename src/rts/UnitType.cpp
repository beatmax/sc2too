#include "rts/UnitType.h"

#include <algorithm>
#include <cassert>

rts::AbilityInstanceIndex rts::UnitType::abilityIndex(AbilityId ability) const {
  auto it = std::find_if(abilities.begin(), abilities.end(), [ability](const auto& ai) {
    return ai.abilityId == ability;
  });
  return (it != abilities.end()) ? AbilityInstanceIndex(it - abilities.begin())
                                 : AbilityInstanceIndex::None;
}

rts::AbilityInstanceIndex rts::UnitType::abilityIndex(abilities::Kind kind) const {
  auto it = std::find_if(
      abilities.begin(), abilities.end(), [kind](const auto& ai) { return ai.kind == kind; });
  return (it != abilities.end()) ? AbilityInstanceIndex(it - abilities.begin())
                                 : AbilityInstanceIndex::None;
}

rts::AbilityStateIndex rts::UnitType::abilityStateIndex(abilities::Kind kind) const {
  auto it = std::find_if(
      abilities.begin(), abilities.end(), [kind](const auto& ai) { return ai.kind == kind; });
  return (it != abilities.end()) ? it->stateIndex : AbilityStateIndex::None;
}

rts::AbilityStateIndex rts::UnitType::abilityStateIndexFor(abilities::Kind kind) const {
  if (kind == abilities::Kind::None)
    return AbilityStateIndex::None;

  AbilityStateIndex stateIndex{0};
  for (const abilities::Instance& ai : abilities) {
    if (ai.kind != abilities::Kind::None) {
      if (ai.kind == kind) {
        stateIndex = ai.stateIndex;
        break;
      }
      if (stateIndex <= ai.stateIndex)
        stateIndex = AbilityStateIndex(ai.stateIndex + 1);
    }
  }
  assert(stateIndex < MaxUnitAbilityStates);
  return stateIndex;
}
