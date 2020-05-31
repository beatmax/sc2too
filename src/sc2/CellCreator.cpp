#include "sc2/CellCreator.h"

#include "rts/Blocker.h"
#include "sc2/ui.h"

#include <memory>

rts::Map::Cell sc2::CellCreator::operator()(char c) const {
  switch (c) {
    case 'r':
      return rts::Blocker{std::make_unique<ui::Rock>()};
    default:
      return rts::Map::Free{};
  }
}
