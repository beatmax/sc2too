#pragma once

#include "util/geo.h"

#include <cstdint>
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

  using BlockerSPtr = std::shared_ptr<Blocker>;
  using BlockerSCPtr = std::shared_ptr<const Blocker>;
  using EntitySPtr = std::shared_ptr<Entity>;
  using EntitySCPtr = std::shared_ptr<const Entity>;
  using EntityWPtr = std::weak_ptr<Entity>;
  using EntityWCPtr = std::weak_ptr<const Entity>;
  using ResourceCPtr = const Resource*;
  using ResourceFieldSPtr = std::shared_ptr<ResourceField>;
  using ResourceFieldSCPtr = std::shared_ptr<const ResourceField>;
  using SideCPtr = const Side*;
  using WorldObjectCPtr = const WorldObject*;
  using WorldObjectSPtr = std::shared_ptr<WorldObject>;
  using WorldObjectSCPtr = std::shared_ptr<const WorldObject>;

  using Distance = uint32_t;   // = 1/1000 cell width/height
  using GameTime = uint32_t;   // = milliseconds at normal speed (1000 Hz)
  using GameSpeed = uint32_t;  // game units per second
  using Speed = uint32_t;      // distance per game unit = cells per second
  using Fps = uint32_t;
  using Quantity = uint32_t;

  constexpr Distance CellDistance{1000};
  constexpr GameTime GameTimeSecond{1000};
  constexpr GameSpeed GameSpeedNormal{1000};

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
