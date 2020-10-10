#include "sc2/MapInitializer.h"

#include "rts/World.h"
#include "sc2/Factory.h"
#include "util/algorithm.h"

#include <cassert>
#include <iterator>

rts::Cell::Content sc2::MapInitializer::operator()(
    rts::World& w, rts::Point p, char c, const std::vector<std::string>& lines) const {
  static auto side1It = w.sides.begin();
  static auto side2It = std::next(side1It);
  const auto side{w.sides.id(
      (p.y >= rts::Coordinate(lines.size() / 2) && side2It != w.sides.end()) ? *side2It
                                                                             : *side1It)};
  switch (c) {
    case 'g':
      return Factory::geyser(w);
    case 'm':
      return Factory::mineralPatch(w, mineralGroup(w, p));
    case 'n':
      return Factory::nexus(w, side);
    case 'p':
      return Factory::pylon(w, side);
    case 'r':
      if (util::allOf(rts::Rectangle{p, {2, 2}}.points(), [&](rts::Point q) {
            return rts::Coordinate(lines.size()) > q.y &&
                rts::Coordinate(lines[q.y].size()) > q.x && lines[q.y][q.x] == 'r' &&
                w.map[q].empty();
          }))
        return Factory::rock(w, {2, 2});
      else
        return Factory::rock(w, {1, 1});
    default:
      return rts::Cell::Empty{};
  }
}

rts::ResourceGroupId sc2::MapInitializer::mineralGroup(const rts::World& w, rts::Point p) const {
  auto rp{util::findIfOpt(boundingBox(p).points(), [&w](rts::Point q) {
    if (auto* rf{w.resourceField(q)})
      return rf->group != 0;
    return false;
  })};
  if (rp)
    return w.resourceField(*rp)->group;
  else
    return ++mineralGroupId_;
}
