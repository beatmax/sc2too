#pragma once

#include "Map.h"
#include "WorldAction.h"
#include "types.h"

#include <utility>

namespace rts {

  class World {
  public:
    explicit World(Map m) : map{std::move(m)} {}

    Map map;
    GameTime time{};

    void update(const WorldActionList& actions);

    void add(EntitySPtr e);
    void destroy(EntityWCPtr e);
    void moveTowards(Position p, EntityWPtr e);
  };
}
