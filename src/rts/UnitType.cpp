#include "rts/UnitType.h"

#include <algorithm>

rts::AbilityInstanceIndex rts::UnitType::abilityIndex(AbilityId ability) const {
  auto it = std::find_if(abilities.begin(), abilities.end(), [ability](const auto& ai) {
    return ai.abilityId == ability;
  });
  return (it != abilities.end()) ? AbilityInstanceIndex(it - abilities.begin())
                                 : AbilityInstanceIndex::None;
}

rts::AbilityStateIndex rts::UnitType::abilityStateIndex(abilities::Kind kind) const {
  auto it = std::find_if(
      abilities.begin(), abilities.end(), [kind](const auto& ai) { return ai.kind == kind; });
  return (it != abilities.end()) ? it->stateIndex : AbilityStateIndex::None;
}

rts::AbilityStateIndex rts::UnitType::abilityStateIndexFor(abilities::Kind kind) const {
  AbilityStateIndex last{AbilityStateIndex::None};
  for (const abilities::Instance& ai : abilities) {
    if (ai.kind != abilities::Kind::None) {
      if (ai.kind == kind)
        return ai.stateIndex;
      else
        last = ai.stateIndex;
    }
  }
  return AbilityStateIndex((last == AbilityStateIndex::None) ? 0 : last + 1);
}
