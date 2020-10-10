#pragma once

#include "types.h"

#include <deque>
#include <utility>

namespace rts {

  using Path = std::deque<Point>;

  std::pair<Path, bool> findPath(const World& w, Point start, Point goal);
  std::pair<Path, bool> findPath(const World& w, Point start, const Rectangle& goalArea);
  std::pair<Path, bool> findPath(
      const World& w, Point start, Point goal, const Rectangle& goalArea);
  std::pair<Path, bool> findPath(const World& w, Point start, UnitId unit);
  std::pair<Path, bool> findPathToTarget(const World& w, Point start, const AbilityTarget& target);

}
