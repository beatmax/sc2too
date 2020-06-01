#pragma once

#include "rts/Entity.h"
#include "rts/Map.h"
#include "rts/types.h"

#include <string>

namespace test {
  extern const std::string map;

  class Simpleton : public rts::EntityTpl<Simpleton, rts::Ui> {
    using Inherited = rts::EntityTpl<Simpleton, rts::Ui>;

  public:
    explicit Simpleton(rts::Point p);
  };

  class Building : public rts::EntityTpl<Building, rts::Ui> {
    using Inherited = rts::EntityTpl<Building, rts::Ui>;

  public:
    explicit Building(rts::Point p);
  };

  class CellCreator : public rts::CellCreator {
  public:
    rts::Map::Cell operator()(char c, rts::Point p) const final;
  };

  char repr(const rts::Ui& ui);
}
