#pragma once

#include "util/geo.h"

#include <cstdint>
#include <memory>

namespace rts {

  class Blocker;
  class Entity;
  class Resource;
  class ResourceField;
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
  using WorldObjectSPtr = std::shared_ptr<WorldObject>;
  using WorldObjectSCPtr = std::shared_ptr<const WorldObject>;

  using GameTime = uint32_t;  // = centiseconds in normal speed (100 Hz)
  using Quantity = uint32_t;

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
