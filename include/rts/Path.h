#pragma once

#include "types.h"

#include <deque>
#include <utility>

namespace rts {

  using Path = std::deque<Point>;

  std::pair<Path, bool> findPath(const World& world, Point start, Point goal);

}
