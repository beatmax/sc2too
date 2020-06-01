#pragma once

#include "util/geo.h"

#include <cstdint>
#include <memory>

namespace rts {

  class Entity;
  class World;

  using EntitySPtr = std::shared_ptr<Entity>;
  using EntitySCPtr = std::shared_ptr<const Entity>;
  using EntityWPtr = std::weak_ptr<Entity>;
  using EntityWCPtr = std::weak_ptr<const Entity>;

  using GameTime = uint32_t;  // = centiseconds in normal speed (100 Hz)

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
