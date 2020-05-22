#include "rts/types.h"

#include <ostream>

std::ostream& rts::operator<<(std::ostream& os, const Position& p) {
  return os << '(' << p.x << ", " << p.y << ')';
}
