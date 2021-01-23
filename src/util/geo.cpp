#include "util/geo.h"

#include "util/algorithm.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ostream>

auto util::geo::operator+(Point p, const VectorList& vs) -> PointList {
  return util::transform(vs, [p](Vector v) { return p + v; });
}

float util::geo::diagonalDistance(Point a, Point b) {
  auto dx{abs(a.x - b.x)};
  auto dy{abs(a.y - b.y)};
  return float(dx + dy) + (DiagD - 2.) * float(std::min(dx, dy));
}

auto util::geo::Circle::points(Cache& c) const -> PointList {
  return center + c.circle(radius, offset2);
}

auto util::geo::rectangleCenteredAt(Point center, Vector size, const Rectangle& bounds)
    -> Rectangle {
  auto r{rectangleCenteredAt(center, size)};
  auto i{intersection(r, bounds)};
  if (i.size.x < r.size.x) {
    if (r.topLeft.x < i.topLeft.x)
      r.topLeft.x = i.topLeft.x;
    else
      r.topLeft.x -= (r.size.x - i.size.x);
  }
  if (i.size.y < r.size.y) {
    if (r.topLeft.y < i.topLeft.y)
      r.topLeft.y = i.topLeft.y;
    else
      r.topLeft.y -= (r.size.y - i.size.y);
  }
  return r;
}

auto util::geo::circleCenteredAt(const Rectangle& r, Coordinate radius) -> Circle {
  return Circle{r.center(), radius, {(r.size.x - 1) % 2, (r.size.y - 1) % 2}};
}

auto util::geo::clamp(Point p, const Rectangle& r) -> Point {
  const auto& tl{r.topLeft};
  const auto& br{r.bottomRight()};
  return {std::clamp(p.x, tl.x, br.x), std::clamp(p.y, tl.y, br.y)};
}

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

auto util::geo::maybeIntersection(const Rectangle& r1, const Rectangle& r2)
    -> std::optional<Rectangle> {
  const auto& bro1{r1.bottomRightOut()};
  const auto& bro2{r2.bottomRightOut()};
  Point topLeft{std::max(r1.topLeft.x, r2.topLeft.x), std::max(r1.topLeft.y, r2.topLeft.y)};
  Point bottomRightOut{std::min(bro1.x, bro2.x), std::min(bro1.y, bro2.y)};
  const auto size = bottomRightOut - topLeft;
  if (size.x > 0 && size.y > 0)
    return Rectangle{topLeft, size};
  return std::nullopt;
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

auto util::geo::Cache::circle(Coordinate radius, Vector offset2) -> const VectorList& {
  auto [it, b] = circles_.try_emplace({radius, offset2.x, offset2.y});
  auto& vectors{it->second};
  if (!b)
    return vectors;

  const float xoff{float(offset2.x) / 2.f};
  const float yoff{float(offset2.y) / 2.f};
  const float fr(radius);

  for (Coordinate y{-radius}; y <= radius; ++y) {
    for (Coordinate x{-radius}; x <= radius; ++x) {
      float fx{float(x) - xoff}, fy{float(y) - yoff};
      if (sqrtf(fx * fx + fy * fy) <= fr)
        vectors.push_back({x, y});
    }
  }

  return vectors;
}

std::ostream& util::geo::operator<<(std::ostream& os, Point p) {
  return os << '(' << p.x << ", " << p.y << ')';
}

std::ostream& util::geo::operator<<(std::ostream& os, Vector v) {
  return os << '(' << v.x << ", " << v.y << ')';
}

std::ostream& util::geo::operator<<(std::ostream& os, FVector v) {
  return os << '(' << v.x << ", " << v.y << ')';
}

std::ostream& util::geo::operator<<(std::ostream& os, const Rectangle& r) {
  return os << "[topLeft=" << r.topLeft << " size=" << r.size << ']';
}
