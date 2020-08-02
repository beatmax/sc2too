#include "util/geo.h"

#include <algorithm>
#include <cassert>
#include <ostream>

auto util::geo::intersection(const Rectangle& r1, const Rectangle& r2) -> Rectangle {
  const auto& br1{r1.topLeft + r1.size};
  const auto& br2{r2.topLeft + r2.size};
  Point topLeft{std::max(r1.topLeft.x, r2.topLeft.x), std::max(r1.topLeft.y, r2.topLeft.y)};
  Point bottomRight{std::min(br1.x, br2.x), std::min(br1.y, br2.y)};
  const auto size = bottomRight - topLeft;
  assert(size.x >= 0);
  assert(size.y >= 0);
  return {topLeft, size};
}

auto util::geo::fixNegativeSize(Rectangle r) -> Rectangle {
  if (r.size.x <= 0) {
    r.size.x = -r.size.x + 1;
    r.topLeft.x -= r.size.x;
  }
  if (r.size.y <= 0) {
    r.size.y = -r.size.y + 1;
    r.topLeft.y -= r.size.y;
  }
  return r;
}

std::ostream& util::geo::operator<<(std::ostream& os, const Point& p) {
  return os << '(' << p.x << ", " << p.y << ')';
}

std::ostream& util::geo::operator<<(std::ostream& os, const Vector& v) {
  return os << '(' << v.x << ", " << v.y << ')';
}

std::ostream& util::geo::operator<<(std::ostream& os, const Rectangle& r) {
  return os << "[topLeft=" << r.topLeft << " size=" << r.size << ']';
}
