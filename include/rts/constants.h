#pragma once

#include "types.h"

#include <cstddef>
#include <limits>

//#define MAP_DEBUG

namespace rts {
  constexpr size_t MaxResources{10};
  constexpr size_t MaxSides{8};
  constexpr size_t MaxSideUnits{1000};
  constexpr size_t MaxUnits{MaxSideUnits * MaxSides};
  constexpr size_t MaxUnitTypes{150};
  constexpr size_t MaxUnitAbilityPages{3};
  constexpr size_t MaxUnitAbilitiesPerPage{15};
  constexpr size_t MaxUnitAbilities{MaxUnitAbilityPages * MaxUnitAbilitiesPerPage};
  constexpr size_t MaxUnitAbilityStates{5};
  constexpr size_t MaxAbilities{1500};
  constexpr size_t MaxBlockers{10000};
  constexpr size_t MaxResourceFields{1000};
  constexpr size_t MaxControlGroups{10};
  constexpr size_t MaxProductionQueues{MaxUnits};
  constexpr size_t MaxProductionQueueSize{5};
  constexpr size_t MaxMessageSize{79};
  constexpr size_t MaxMessages{4};
  constexpr size_t MaxEnqueuedCommands{20};

  constexpr Distance CellDistance{100};
  constexpr Distance DiagonalDistance{141};
  constexpr GameTime GameTimeSecond{100};
  constexpr GameSpeed GameSpeedNormal{100};
  constexpr GameSpeed GameSpeedFaster{140};
  constexpr Coordinate ResourceProximityRadius{3};

  constexpr Distance DistanceInf{std::numeric_limits<Distance>::max()};
  constexpr GameTime GameTimeInf{std::numeric_limits<GameTime>::max()};
  constexpr Quantity QuantityInf{std::numeric_limits<Quantity>::max()};

  constexpr LayerId LayerBottom{0};
  constexpr LayerId LayerPrototypes{1};
  constexpr LayerId LayerUnits{2};
  constexpr LayerId LayerCount{3};
}
