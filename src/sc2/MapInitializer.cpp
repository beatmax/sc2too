#include "sc2/MapInitializer.h"

#include "rts/World.h"
#include "sc2/Factory.h"

#include <cassert>

void sc2::MapInitializer::operator()(rts::World& world, rts::Point p, char c) const {
  assert(!world.sides.empty());
  switch (c) {
    case 'g':
      world.add(Factory::geyser(p));
      break;
    case 'm':
      world.add(Factory::mineralPatch(p));
      break;
    case 'n':
      world.add(Factory::nexus(p, &world.sides[nexusCnt_++ % world.sides.size()]));
      return;
    case 'r':
      world.add(Factory::rock(p));
      break;
  }
}
