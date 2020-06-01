#include "sc2/CellCreator.h"

#include "rts/Blocker.h"
#include "sc2/Nexus.h"
#include "sc2/ui.h"

#include <memory>

rts::Map::Cell sc2::CellCreator::operator()(char c, rts::Point p) const {
  switch (c) {
    case 'n':
      return sc2::Nexus::create(p);
    case 'r':
      return rts::Blocker{std::make_unique<ui::Rock>()};
    default:
      return rts::Map::Free{};
  }
}
