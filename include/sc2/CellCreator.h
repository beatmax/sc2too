#pragma once

#include "rts/Map.h"

namespace sc2 {
  class CellCreator : public rts::CellCreator {
  public:
    rts::Map::Cell operator()(const rts::World& world, rts::Point p, char c) const final;

  private:
    mutable size_t nexusCnt_{};
  };
}
