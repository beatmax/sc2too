#include "sc2/CellCreator.h"

#include "sc2/Geyser.h"
#include "sc2/MineralPatch.h"
#include "sc2/Nexus.h"
#include "sc2/Rock.h"

#include <memory>

rts::Map::Cell sc2::CellCreator::operator()(char c, rts::Point p) const {
  switch (c) {
    case 'g':
      return sc2::Geyser::create(p);
    case 'm':
      return sc2::MineralPatch::create(p);
    case 'n':
      return sc2::Nexus::create(p);
    case 'r':
      return sc2::Rock::create(p);
    default:
      return rts::Map::Free{};
  }
}
