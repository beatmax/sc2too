#include "rts/World.h"

#include "rts/Entity.h"
#include "util/misc.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <utility>

namespace rts {
  namespace {
    constexpr size_t MaxSides{8};
    constexpr size_t MaxEntities{200 * MaxSides};

    // TODO something more efficient
    EntitySPtr& findEntity(std::vector<EntitySPtr>& v, const Entity* e) {
      auto it = std::find_if(v.begin(), v.end(), [e](const auto& p) { return p.get() == e; });
      return *it;
    }
  }
}

rts::World::World(std::vector<Side> s, const CellCreator& creator, std::istream&& is)
  : sides{std::move(s)}, entities(MaxEntities), map{*this, creator, std::move(is)} {
}

void rts::World::update(const WorldActionList& actions) {
  for (auto& action : actions)
    action(*this);
}

void rts::World::add(EntitySPtr e) {
  map.set(e->area, e);
  findEntity(entities, {}) = e;
}

void rts::World::destroy(EntityWCPtr e) {
  if (auto entity = e.lock()) {
    assert(hasEntity(map.at(entity->area.topLeft)));
    map.set(entity->area, Map::Free{});
    findEntity(entities, entity.get()).reset();
  }
}

void rts::World::moveTowards(Point p, EntityWPtr e) {
  if (auto entity = e.lock()) {
    assert(entity->area.size == Vector({1, 1}));  // no need to move big entities so far
    Point pos{entity->area.topLeft};
    assert(hasEntity(map.at(pos)));

    Vector v{p - pos};
    if (abs(v.x) > abs(v.y))
      pos += Vector{util::unit(v.x), 0};
    else
      pos += Vector{0, util::unit(v.y)};

    if (isFree(map.at(pos))) {
      auto& epos = entity->area.topLeft;
      map.set(epos, Map::Free{});
      map.set(pos, std::move(entity));
      epos = pos;
    }
  }
}
