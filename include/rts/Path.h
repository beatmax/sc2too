#pragma once

#include "types.h"

#include <deque>
#include <unordered_set>
#include <utility>

namespace rts {

  using Path = std::deque<Point>;
  using ExcludedPointSet = std::unordered_set<Point>;

  std::pair<Path, bool> findPath(
      const World& w, Point start, Point goal, const ExcludedPointSet& excl);
  std::pair<Path, bool> findPath(
      const World& w, Point start, const Rectangle& goalArea, const ExcludedPointSet& excl);
  std::pair<Path, bool> findPath(
      const World& w,
      Point start,
      Point goal,
      const Rectangle& goalArea,
      const ExcludedPointSet& excl);
  std::pair<Path, bool> findPath(
      const World& w, Point start, UnitId unit, const ExcludedPointSet& excl);
  std::pair<Path, bool> findPathToTarget(
      const World& w, Point start, const AbilityTarget& target, const ExcludedPointSet& excl);

  bool dodge(const World& w, Point pos, Path& path);

}
