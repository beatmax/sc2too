#include "rts/World.h"

#include "rts/Entity.h"

void rts::World::add(EntitySPtr e) {
  map.set(e->position, std::move(e));
}
