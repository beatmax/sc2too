#include "rts/ProximityMap.h"

#include "rts/World.h"
#include "util/algorithm.h"

rts::PointList rts::ProximityMap::update(World& w, const Circle& c, int16_t inc) {
  auto points{util::filter(c.points(w.geoCache), [a{area()}](Point p) { return a.contains(p); })};

  for (Point p : points)
    (*this)[p] += inc;

  return points;
}

void rts::ProximityMap::updateContour(
    World& w, const Rectangle& r, Coordinate radius, int16_t inc) {
  for (Point op : r.outerPoints()) {
    Circle c{op, radius};
    auto points{util::filter(c.points(w.geoCache), [ma{area()}, &r](Point p) {
      return ma.contains(p) && !r.contains(p);
    })};

    for (Point p : points)
      (*this)[p] += inc;
  }
}
