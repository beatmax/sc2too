#pragma once

#include "types.h"

namespace rts {
  constexpr Distance CellDistance{100};
  constexpr Distance DiagonalDistance{141};
  constexpr GameTime GameTimeSecond{100};
  constexpr GameTime GameTimeInf{std::numeric_limits<GameTime>::max()};
  constexpr GameSpeed GameSpeedNormal{100};

  inline Distance adjacentDistance(const Point& p1, const Point& p2) {
    return (p1.x == p2.x || p1.y == p2.y) ? CellDistance : DiagonalDistance;
  }
}
