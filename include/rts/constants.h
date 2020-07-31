#pragma once

#include <cstddef>

namespace rts {
  constexpr static size_t MaxSides{8};
  constexpr static size_t MaxSideUnits{200};
  constexpr static size_t MaxSideStructures{1000};
  constexpr static size_t MaxSideEntities{MaxSideUnits + MaxSideStructures};
  constexpr static size_t MaxEntities{MaxSideEntities * MaxSides};
  constexpr static size_t MaxEntityTypes{1000};
  constexpr static size_t MaxAbilities{1000};
  constexpr static size_t MaxEntityAbilities{12};
  constexpr static size_t MaxBlockers{10000};
  constexpr static size_t MaxResourceFields{10000};
}
