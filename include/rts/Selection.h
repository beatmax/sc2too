#pragma once

#include "World.h"
#include "util/WeakList.h"

#include <utility>
#include <vector>

namespace rts {
  class Selection {
  public:
    using IdList = std::vector<EntityId>;
    using CPtrList = std::vector<EntityCPtr>;

    void set(const World& world, IdList ids) { list_.set(world.entities, std::move(ids)); }
    void add(const World& world, IdList ids) { list_.add(world.entities, std::move(ids)); }
    void remove(const IdList& ids) { list_.remove(ids); }
    CPtrList items(const World& world) const { return list_.items(world.entities); }

  private:
    struct Compare {
      bool operator()(const Entity& e1, const Entity& e2) const { return &e1 < &e2; }
    };

    util::WeakList<Entity, World::MaxSideEntities, Compare> list_;
  };
}
