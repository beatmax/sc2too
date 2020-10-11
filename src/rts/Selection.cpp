#include "rts/Selection.h"

#include "rts/World.h"
#include "util/algorithm.h"

#include <algorithm>

namespace rts {
  namespace {
    template<typename It, typename Pred>
    UnitTypeId nextType(It first, It last, Pred p) {
      auto it{std::partition_point(first, last, p)};
      if (it == last)
        it = first;
      return (it != last) ? (*it)->type : UnitTypeId{};
    }
  }
}

bool rts::Selection::Subgroup::abilityEnabled(const World& w, AbilityInstanceIndex ai) const {
  const auto* a{abilities[ai]};
  if (a && a->abilityId) {
    auto units{w.units[allIds]};
    return util::anyOf(units, [&w, ai{AbilityInstanceIndex{ai}}, id{a->abilityId}](const Unit* u) {
      return u->hasEnabledAbility(w, ai, id);
    });
  }
  return false;
}

std::vector<bool> rts::Selection::Subgroup::abilityEnabled(const World& w) const {
  std::vector<bool> enabled(MaxUnitAbilities, false);
  auto units{w.units[allIds]};
  for (size_t ai{0}; ai < MaxUnitAbilities; ++ai) {
    const auto* a{abilities[ai]};
    if (a && a->abilityId) {
      enabled[ai] =
          util::anyOf(units, [&w, ai{AbilityInstanceIndex{ai}}, id{a->abilityId}](const Unit* u) {
            return u->hasEnabledAbility(w, ai, id);
          });
    }
  }
  return enabled;
}

void rts::Selection::set(const World& w, UnitIdList ids) {
  Group::set(w, ids);
  subgroupReset(w);
}

void rts::Selection::add(const World& w, UnitIdList ids) {
  Group::add(w, ids);
  if (!subgroup_.type)
    subgroupReset(w);
}

void rts::Selection::remove(const World& w, UnitIdList ids) {
  Group::remove(ids);
  if (!subgroup(w).type)
    subgroupReset(w);
}

auto rts::Selection::subgroup(const World& w) const -> const Subgroup& {
  subgroup_.allIds = Group::ids(w);
  auto units{w.units[subgroup_.allIds]};
  auto it{std::partition_point(
      units.begin(), units.end(), [this](const Unit* u) { return u->type < subgroup_.type; })};
  if (it == units.end() || (*it)->type != subgroup_.type)
    subgroup_.type = {};
  return subgroup_;
}

void rts::Selection::subgroupNext(const World& w) {
  auto units{items(w)};
  setSubgroup(w, nextType(units.begin(), units.end(), [this](const Unit* u) {
                return u->type <= subgroup_.type;
              }));
}

void rts::Selection::subgroupPrevious(const World& w) {
  auto units{items(w)};
  setSubgroup(w, nextType(units.rbegin(), units.rend(), [this](const Unit* u) {
                return subgroup_.type <= u->type;
              }));
}

void rts::Selection::subgroupReset(const World& w) {
  auto sortedIds{Group::ids(w)};
  setSubgroup(w, sortedIds.empty() ? UnitTypeId{} : w[sortedIds.front()].type);
}

void rts::Selection::setSubgroup(const World& w, UnitTypeId type) {
  subgroup_.type = type;

  if (subgroup_.type) {
    const auto& t{w[subgroup_.type]};
    for (size_t ai{0}; ai < MaxUnitAbilities; ++ai)
      subgroup_.abilities[ai] = &t.abilities[ai];
    subgroup_.defaultAbility = t.defaultAbility;
  }
  else {
    for (size_t ai{0}; ai < MaxUnitAbilities; ++ai)
      subgroup_.abilities[ai] = nullptr;
    subgroup_.defaultAbility = {};
  }
}
