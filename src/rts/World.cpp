#include "rts/World.h"

#include "rts/Entity.h"
#include "util/misc.h"

#include <cassert>
#include <cstdlib>

void rts::World::update(const WorldActionList& actions) {
  for (auto& action : actions)
    action(*this);
}

void rts::World::add(EntitySPtr e) {
  map.set(e->area, e);
}

void rts::World::destroy(EntityWCPtr e) {
  if (auto entity = e.lock()) {
    assert(hasEntity(map.at(entity->area.topLeft)));
    map.set(entity->area, Map::Free{});
  }
}

void rts::World::moveTowards(Point p, EntityWPtr e) {
  if (auto entity = e.lock()) {
    assert(entity->area.size == Vector({1, 1}));  // no need to move big entities so far
    Point& epos = entity->area.topLeft;
    assert(hasEntity(map.at(epos)));
    map.set(epos, Map::Free{});
    Vector v{p - epos};
    if (abs(v.x) > abs(v.y))
      epos.x += util::unit(v.x);
    else
      epos.y += util::unit(v.y);
    map.set(epos, std::move(entity));
  }
}
