#pragma once

#include "constants.h"
#include "types.h"
#include "util/WeakList.h"

#include <utility>
#include <vector>

namespace rts {
  class Group {
  public:
    void set(const World& w, EntityIdList ids);
    void add(const World& w, EntityIdList ids);
    void remove(const EntityIdList& ids);
    EntityIdList ids(const World& w) const;
    EntityCPtrList items(const World& w) const;
    bool contains(EntityId id) const { return list_.contains(id); }

  private:
    struct Compare {
      bool operator()(const Entity& e1, const Entity& e2) const;
    };

    util::WeakList<Entity, MaxSideEntities, Compare> list_;
  };
}
