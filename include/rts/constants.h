#pragma once

#include "types.h"

#include <cstddef>
#include <limits>

//#define MAP_DEBUG

namespace rts {
  constexpr static size_t MaxResources{10};
  constexpr static size_t MaxSides{8};
  constexpr static size_t MaxSideUnits{1000};
  constexpr static size_t MaxUnits{MaxSideUnits * MaxSides};
  constexpr static size_t MaxUnitTypes{150};
  constexpr static size_t MaxUnitAbilities{15};
  constexpr static size_t MaxAbilities{1500};
  constexpr static size_t MaxBlockers{10000};
  constexpr static size_t MaxResourceFields{1000};
  constexpr static size_t MaxControlGroups{10};
  constexpr static size_t MaxProductionQueues{MaxUnits};
  constexpr static size_t MaxProductionQueueSize{5};
  constexpr static size_t MaxMessageSize{79};
  constexpr static size_t MaxMessages{4};

  constexpr Distance CellDistance{100};
  constexpr Distance DiagonalDistance{141};
  constexpr GameTime GameTimeSecond{100};
  constexpr GameSpeed GameSpeedNormal{100};

  constexpr Distance DistanceInf{std::numeric_limits<Distance>::max()};
  constexpr GameTime GameTimeInf{std::numeric_limits<GameTime>::max()};
  constexpr Quantity QuantityInf{std::numeric_limits<Quantity>::max()};
}
