#pragma once

#include "Map.h"
#include "types.h"

#include <utility>

namespace rts {

  class World {
  public:
    explicit World(Map m) : map{std::move(m)} {}

    Map map;

    void add(EntitySPtr e);
  };
}
