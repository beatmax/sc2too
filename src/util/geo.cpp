#include "util/geo.h"

#include <ostream>

std::ostream& util::geo::operator<<(std::ostream& os, const Point& p) {
  return os << '(' << p.x << ", " << p.y << ')';
}

std::ostream& util::geo::operator<<(std::ostream& os, const Vector& v) {
  return os << '(' << v.x << ", " << v.y << ')';
}

std::ostream& util::geo::operator<<(std::ostream& os, const Rectangle& r) {
  return os << "[topLeft=" << r.topLeft << " size=" << r.size << ']';
}
