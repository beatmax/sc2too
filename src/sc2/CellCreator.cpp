#include "sc2/CellCreator.h"

#include "rts/World.h"
#include "sc2/Geyser.h"
#include "sc2/MineralPatch.h"
#include "sc2/Nexus.h"
#include "sc2/Rock.h"

#include <cassert>

rts::Map::Cell sc2::CellCreator::operator()(const rts::World& world, rts::Point p, char c) const {
  assert(!world.sides.empty());
  switch (c) {
    case 'g':
      return sc2::Geyser::create(p);
    case 'm':
      return sc2::MineralPatch::create(p);
    case 'n':
      return sc2::Nexus::create(p, &world.sides[nexusCnt_++ % world.sides.size()]);
    case 'r':
      return sc2::Rock::create(p);
    default:
      return rts::Map::Free{};
  }
}
