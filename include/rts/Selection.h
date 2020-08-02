#pragma once

#include "constants.h"
#include "types.h"
#include "util/WeakList.h"

#include <utility>
#include <vector>

namespace rts {
  class Selection {
  public:
    void set(const World& world, EntityIdList ids);
    void add(const World& world, EntityIdList ids);
    void remove(const EntityIdList& ids);
    EntityCPtrList items(const World& world) const;
    bool contains(EntityId id) const { return list_.contains(id); }

  private:
    struct Compare {
      bool operator()(const Entity& e1, const Entity& e2) const { return &e1 < &e2; }
    };

    util::WeakList<Entity, MaxSideEntities, Compare> list_;
  };
}
