#pragma once

#include "rts/Map.h"

namespace sc2 {
  class CellCreator : public rts::CellCreator {
  public:
    rts::Map::Cell operator()(char c) const final;
  };
}
