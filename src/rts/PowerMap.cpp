#include "rts/PowerMap.h"

#include "rts/UnitType.h"
#include "rts/World.h"
#include "util/algorithm.h"

void rts::PowerMap::update(World& w, SideId side, const Circle& c, int16_t inc) {
  auto points{ProximityMap::update(w, c, inc)};

  for (auto id : w.unitsInPoints(points, side, UnitType::Kind::Structure)) {
    auto& u{w[id]};
    if (w[u.type].requiresPower == UnitType::RequiresPower::Yes)
      u.powered = isActive(u.area.center());
  }
}
