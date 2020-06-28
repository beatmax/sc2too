#pragma once

#include "util/Pool.h"
#include "util/geo.h"

#include <cstdint>
#include <limits>
#include <memory>

namespace rts {

  class Blocker;
  class Engine;
  class Entity;
  class Resource;
  class ResourceField;
  class Side;
  class World;
  class WorldObject;

  using BlockerId = util::PoolObjectId<Blocker>;
  using BlockerWId = util::PoolObjectWeakId<Blocker>;
  using EntityId = util::PoolObjectId<Entity>;
  using EntityWId = util::PoolObjectWeakId<Entity>;
  using ResourceCPtr = const Resource*;
  using ResourceFieldId = util::PoolObjectId<ResourceField>;
  using ResourceFieldWId = util::PoolObjectWeakId<ResourceField>;
  using SideCPtr = const Side*;
  using WorldObjectCPtr = const WorldObject*;

  using Distance = uint32_t;   // = 1/100 cell width/height
  using GameTime = uint32_t;   // = centiseconds at normal speed (100 Hz)
  using GameSpeed = uint32_t;  // game units per second
  using Speed = uint32_t;      // distance per game unit = cells per second
  using Fps = uint32_t;
  using Quantity = uint32_t;

  constexpr Distance CellDistance{100};
  constexpr GameTime GameTimeSecond{100};
  constexpr GameTime GameTimeInf{std::numeric_limits<GameTime>::max()};
  constexpr GameSpeed GameSpeedNormal{100};

  using util::geo::Coordinate;
  using util::geo::Point;
  using util::geo::Rectangle;
  using util::geo::Vector;

  class Ui {
  public:
    virtual ~Ui() = default;
  };

  using UiUPtr = std::unique_ptr<Ui>;
}
