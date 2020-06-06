#pragma once

#include "Map.h"
#include "Side.h"
#include "WorldAction.h"
#include "types.h"

#include <vector>

namespace rts {

  class World {
  public:
    World(Map m, size_t numSides, ResourceMap resources);

    Map map;
    GameTime time{};
    std::vector<Side> sides;

    void update(const WorldActionList& actions);

    void add(EntitySPtr e);
    void destroy(EntityWCPtr e);
    void moveTowards(Point p, EntityWPtr e);
  };
}
