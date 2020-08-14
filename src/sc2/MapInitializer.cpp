#include "sc2/MapInitializer.h"

#include "rts/World.h"
#include "sc2/Factory.h"

#include <cassert>

void sc2::MapInitializer::operator()(rts::World& w, rts::Point p, char c) const {
  static auto sideIt = w.sides.begin();
  assert(sideIt != w.sides.end());
  switch (c) {
    case 'g':
      Factory::geyser(w, p);
      break;
    case 'm':
      Factory::mineralPatch(w, p, mineralGroup(w, p));
      break;
    case 'n':
      Factory::nexus(w, p, w.sides.id(*sideIt));
      if (++sideIt == w.sides.end())
        sideIt = w.sides.begin();
      return;
    case 'r':
      Factory::rock(w, p);
      break;
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
