#pragma once

#include "Blocker.h"
#include "Entity.h"
#include "ResourceField.h"
#include "Side.h"
#include "WorldObject.h"
#include "util/Matrix.h"

#include <cassert>
#include <iosfwd>
#include <utility>
#include <variant>
#include <vector>

namespace rts {

  class MapInitializer;
  class World;

  class Map {
  public:
    using Free = std::monostate;
    using Cell = std::variant<Free, BlockerId, EntityId, ResourceFieldId>;

    explicit Map(World& world, const MapInitializer& init, std::istream&& is);
    explicit Map(World& world, const MapInitializer& init, const std::vector<std::string>& lines);

    const Coordinate maxX;
    const Coordinate maxY;

    Cell& at(Point p) { return at(p.x, p.y); }
    Cell& at(Coordinate x, Coordinate y) { return cells_(y, x); }
    const Cell& at(Point p) const { return at(p.x, p.y); }
    const Cell& at(Coordinate x, Coordinate y) const { return cells_(y, x); }

    template<typename T>
    void set(Point p, T&& o) {
      cells_(p.y, p.x) = std::forward<T>(o);
    }

    template<typename T>
    void set(const Rectangle& area, const T& o) {
      forEachPoint(area, [this, &o](Point p) { cells_(p.y, p.x) = o; });
    }

  private:
    util::Matrix<Cell, Coordinate> cells_;
  };

  inline bool isFree(const Map::Cell& c) { return std::holds_alternative<Map::Free>(c); }
  inline bool hasObject(const Map::Cell& c) { return !isFree(c); }

  WorldObject& getObject(World& world, Map::Cell& c);
  const WorldObject& getObject(const World& world, const Map::Cell& c);

  WorldObject* getObjectPtr(World& world, Map::Cell& c);
  const WorldObject* getObjectPtr(const World& world, const Map::Cell& c);

  inline bool hasBlocker(const Map::Cell& c) { return std::holds_alternative<BlockerId>(c); }
  inline bool hasEntity(const Map::Cell& c) { return std::holds_alternative<EntityId>(c); }
  inline bool hasResourceField(const Map::Cell& c) {
    return std::holds_alternative<ResourceFieldId>(c);
  }

  inline BlockerId getBlockerId(Map::Cell& c) { return std::get<BlockerId>(c); }
  inline BlockerId getBlockerId(const Map::Cell& c) { return std::get<BlockerId>(c); }
  inline EntityId getEntityId(Map::Cell& c) { return std::get<EntityId>(c); }
  inline EntityId getEntityId(const Map::Cell& c) { return std::get<EntityId>(c); }
  inline ResourceFieldId getResourceFieldId(Map::Cell& c) { return std::get<ResourceFieldId>(c); }
  inline ResourceFieldId getResourceFieldId(const Map::Cell& c) {
    return std::get<ResourceFieldId>(c);
  }

  class MapInitializer {
  public:
    virtual ~MapInitializer() = default;
    virtual void operator()(World& world, Point p, char c) const = 0;
  };
}
