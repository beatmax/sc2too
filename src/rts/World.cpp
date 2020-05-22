#include "rts/World.h"

#include "rts/Entity.h"
#include "util/misc.h"

#include <cstdlib>

void rts::World::update(const WorldActionList& actions) {
  for (auto& action : actions)
    action(*this);
}

void rts::World::add(EntitySPtr e) {
  map.set(e->position, std::move(e));
}

void rts::World::moveTowards(Position p, EntityWPtr e) {
  if (auto entity = e.lock()) {
    Position& epos = entity->position;
    map.set(epos, Map::Free{});
    Vector v{p - epos};
    if (abs(v.x) > abs(v.y))
      epos.x += util::unit(v.x);
    else
      epos.y += util::unit(v.y);
    map.set(epos, std::move(entity));
  }
}
