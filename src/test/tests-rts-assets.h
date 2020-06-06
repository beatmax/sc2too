#pragma once

#include "rts/Entity.h"
#include "rts/Map.h"
#include "rts/Resource.h"
#include "rts/ResourceField.h"
#include "rts/types.h"

#include <string>

namespace test {
  extern const std::string map;
  extern const rts::Resource gas;
  extern const rts::ResourceMap resources;

  struct Ui : rts::Ui {
    char repr;

    explicit Ui(char r) : repr{r} {}
  };

  inline char repr(const rts::Ui& ui) { return static_cast<const Ui&>(ui).repr; }

  class Simpleton : public rts::EntityTpl<Simpleton, Ui> {
    using Inherited = rts::EntityTpl<Simpleton, Ui>;

  public:
    explicit Simpleton(rts::Point p);
  };

  class Building : public rts::EntityTpl<Building, Ui> {
    using Inherited = rts::EntityTpl<Building, Ui>;

  public:
    explicit Building(rts::Point p);
  };

  class Geyser : public rts::ResourceFieldTpl<Geyser, Ui> {
    using Inherited = rts::ResourceFieldTpl<Geyser, Ui>;

  public:
    explicit Geyser(rts::Point p);
  };

  class Rock : public rts::BlockerTpl<Rock, Ui> {
    using Inherited = rts::BlockerTpl<Rock, Ui>;

  public:
    explicit Rock(rts::Point p);
  };

  class CellCreator : public rts::CellCreator {
  public:
    rts::Map::Cell operator()(char c, rts::Point p) const final;
  };
}
