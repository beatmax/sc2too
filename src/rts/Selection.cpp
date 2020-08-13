#include "rts/Selection.h"

#include "rts/World.h"

#include <algorithm>

namespace rts {
  namespace {
    template<typename It, typename Pred>
    EntityTypeId nextType(It first, It last, Pred p) {
      auto it{std::partition_point(first, last, p)};
      if (it == last)
        it = first;
      return (it != last) ? (*it)->type : EntityTypeId{};
    }
  }
}

void rts::Selection::set(const World& w, EntityIdList ids) {
  Group::set(w, ids);
  subgroupReset(w);
}

void rts::Selection::add(const World& w, EntityIdList ids) {
  Group::add(w, ids);
  if (!subgroupType_)
    subgroupReset(w);
}

rts::EntityTypeId rts::Selection::subgroupType(const World& w) const {
  auto entities{items(w)};
  auto it{std::partition_point(entities.begin(), entities.end(), [this](const Entity* e) {
    return e->type < subgroupType_;
  })};
  if (it == entities.end() || (*it)->type != subgroupType_)
    subgroupType_ = {};
  return subgroupType_;
}

void rts::Selection::subgroupNext(const World& w) {
  auto entities{items(w)};
  subgroupType_ = nextType(entities.begin(), entities.end(), [this](const Entity* e) {
    return e->type <= subgroupType_;
  });
}

void rts::Selection::subgroupPrevious(const World& w) {
  auto entities{items(w)};
  subgroupType_ = nextType(entities.rbegin(), entities.rend(), [this](const Entity* e) {
    return subgroupType_ <= e->type;
  });
}

void rts::Selection::subgroupReset(const World& w) {
  auto sortedIds{Group::ids(w)};
  subgroupType_ = sortedIds.empty() ? EntityTypeId{} : w[sortedIds.front()].type;
}
