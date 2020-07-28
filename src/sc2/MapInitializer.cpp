#include "sc2/MapInitializer.h"

#include "rts/World.h"
#include "sc2/Factory.h"

#include <cassert>

void sc2::MapInitializer::operator()(rts::World& world, rts::Point p, char c) const {
  static auto sideIt = world.sides.begin();
  assert(sideIt != world.sides.end());
  switch (c) {
    case 'g':
      Factory::geyser(world, p);
      break;
    case 'm':
      Factory::mineralPatch(world, p);
      break;
    case 'n':
      Factory::nexus(world, p, world.sides.id(*sideIt));
      if (++sideIt == world.sides.end())
        sideIt = world.sides.begin();
      return;
    case 'r':
      Factory::rock(world, p);
      break;
  }
}
