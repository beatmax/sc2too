#include "rts/Selection.h"

#include "rts/World.h"

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

void rts::Selection::set(const World& w, UnitIdList ids) {
  Group::set(w, ids);
  subgroupReset(w);
}

void rts::Selection::add(const World& w, UnitIdList ids) {
  Group::add(w, ids);
  if (!subgroupType_)
    subgroupReset(w);
}

rts::UnitTypeId rts::Selection::subgroupType(const World& w) const {
  auto units{items(w)};
  auto it{std::partition_point(
      units.begin(), units.end(), [this](const Unit* u) { return u->type < subgroupType_; })};
  if (it == units.end() || (*it)->type != subgroupType_)
    subgroupType_ = {};
  return subgroupType_;
}

void rts::Selection::subgroupNext(const World& w) {
  auto units{items(w)};
  subgroupType_ = nextType(
      units.begin(), units.end(), [this](const Unit* u) { return u->type <= subgroupType_; });
}

void rts::Selection::subgroupPrevious(const World& w) {
  auto units{items(w)};
  subgroupType_ = nextType(
      units.rbegin(), units.rend(), [this](const Unit* u) { return subgroupType_ <= u->type; });
}

void rts::Selection::subgroupReset(const World& w) {
  auto sortedIds{Group::ids(w)};
  subgroupType_ = sortedIds.empty() ? UnitTypeId{} : w[sortedIds.front()].type;
}
