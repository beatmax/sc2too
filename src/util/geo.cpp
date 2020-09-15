#include "util/geo.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ostream>

bool util::geo::intersect(const Rectangle& r1, const Rectangle& r2) {
  const auto& bro1{r1.bottomRightOut()};
  const auto& bro2{r2.bottomRightOut()};
  Point topLeft{std::max(r1.topLeft.x, r2.topLeft.x), std::max(r1.topLeft.y, r2.topLeft.y)};
  Point bottomRightOut{std::min(bro1.x, bro2.x), std::min(bro1.y, bro2.y)};
  const auto size = bottomRightOut - topLeft;
  return size.x > 0 && size.y > 0;
}

auto util::geo::intersection(const Rectangle& r1, const Rectangle& r2) -> Rectangle {
  const auto& bro1{r1.bottomRightOut()};
  const auto& bro2{r2.bottomRightOut()};
  Point topLeft{std::max(r1.topLeft.x, r2.topLeft.x), std::max(r1.topLeft.y, r2.topLeft.y)};
  Point bottomRightOut{std::min(bro1.x, bro2.x), std::min(bro1.y, bro2.y)};
  const auto size = bottomRightOut - topLeft;
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

float util::geo::diagonalDistance(Point a, Point b) {
  auto dx{abs(a.x - b.x)};
  auto dy{abs(a.y - b.y)};
  return float(dx + dy) + (DiagD - 2.) * float(std::min(dx, dy));
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
