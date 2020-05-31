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
    explicit Simpleton(rts::Position p);
  };

  class CellCreator : public rts::CellCreator {
  public:
    rts::Map::Cell operator()(char c) const final;
  };

  char repr(const rts::Ui& ui);
}
