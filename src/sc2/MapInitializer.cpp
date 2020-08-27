#include "sc2/MapInitializer.h"

#include "rts/World.h"
#include "sc2/Factory.h"

#include <cassert>

rts::Cell::Content sc2::MapInitializer::operator()(rts::World& w, rts::Point p, char c) const {
  static auto sideIt = w.sides.begin();
  switch (c) {
    case 'g':
      return Factory::geyser(w, p);
    case 'm':
      return Factory::mineralPatch(w, p, mineralGroup(w, p));
    case 'n':
      if (sideIt == w.sides.end())
        sideIt = w.sides.begin();
      return Factory::nexus(w, p, w.sides.id(*sideIt++));
    case 'r':
      return Factory::rock(w, p);
    default:
      return rts::Cell::Empty{};
  }
}

rts::ResourceGroupId sc2::MapInitializer::mineralGroup(const rts::World& w, rts::Point p) const {
  auto rp{findInPoints(boundingBox(p), [&w](rts::Point q) {
    if (auto* rf{w.resourceField(q)})
      return rf->group != 0;
    return false;
  })};
  if (rp)
    return w.resourceField(*rp)->group;
  else
    return ++mineralGroupId_;
}
