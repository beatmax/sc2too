#pragma once

#include "Map.h"
#include "Side.h"
#include "WorldAction.h"
#include "types.h"

#include <vector>

namespace rts {

  class World {
  public:
    World(std::vector<Side> s, const CellCreator& creator, std::istream&& is);

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    GameTime time{};

    std::vector<Side> sides;
    std::vector<EntitySPtr> entities;
    Map map;

    void update(const WorldActionList& actions);

    void add(EntitySPtr e);
    void destroy(EntityWCPtr e);
    void moveTowards(Point p, EntityWPtr e);
  };
}
