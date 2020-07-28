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

#define MAP_DEBUG

namespace rts {

  class MapInitializer;
  class World;

  class Map {
  public:
    using Free = std::monostate;
    using Content = std::variant<Free, BlockerId, EntityId, ResourceFieldId>;

    struct Cell {
      Content content;
#ifdef MAP_DEBUG
      struct {
        int color{0};
      } debug;
#endif
    };

    void load(World& world, const MapInitializer& init, std::istream&& is);
    void load(World& world, const MapInitializer& init, const std::vector<std::string>& lines);

    Map() = default;
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    Coordinate maxX() const { return cells_.cols(); }
    Coordinate maxY() const { return cells_.rows(); }
    size_t size() const { return cells_.size(); }

    Cell& at(Point p) { return at(p.x, p.y); }
    Cell& at(Coordinate x, Coordinate y) { return cells_(y, x); }
    const Cell& at(Point p) const { return at(p.x, p.y); }
    const Cell& at(Coordinate x, Coordinate y) const { return cells_(y, x); }

    template<typename T>
    void setContent(Point p, T&& o) {
      cells_(p.y, p.x).content = std::forward<T>(o);
    }

    template<typename T>
    void setContent(const Rectangle& area, const T& o) {
      forEachPoint(area, [this, &o](Point p) { cells_(p.y, p.x).content = o; });
    }

  private:
    using CellMatrix = util::Matrix<Cell, Coordinate>;

    CellMatrix cells_;
  };

  inline bool isFree(const Map::Cell& c) { return std::holds_alternative<Map::Free>(c.content); }
  inline bool hasObject(const Map::Cell& c) { return !isFree(c); }

  WorldObject& getObject(World& world, Map::Cell& c);
  const WorldObject& getObject(const World& world, const Map::Cell& c);

  WorldObject* getObjectPtr(World& world, Map::Cell& c);
  const WorldObject* getObjectPtr(const World& world, const Map::Cell& c);

  inline bool hasBlocker(const Map::Cell& c) {
    return std::holds_alternative<BlockerId>(c.content);
  }
  inline bool hasEntity(const Map::Cell& c) { return std::holds_alternative<EntityId>(c.content); }
  inline bool hasResourceField(const Map::Cell& c) {
    return std::holds_alternative<ResourceFieldId>(c.content);
  }

  inline BlockerId getBlockerId(Map::Cell& c) { return std::get<BlockerId>(c.content); }
  inline BlockerId getBlockerId(const Map::Cell& c) { return std::get<BlockerId>(c.content); }
  inline EntityId getEntityId(Map::Cell& c) { return std::get<EntityId>(c.content); }
  inline EntityId getEntityId(const Map::Cell& c) { return std::get<EntityId>(c.content); }
  inline ResourceFieldId getResourceFieldId(Map::Cell& c) {
    return std::get<ResourceFieldId>(c.content);
  }
  inline ResourceFieldId getResourceFieldId(const Map::Cell& c) {
    return std::get<ResourceFieldId>(c.content);
  }

  class MapInitializer {
  public:
    virtual ~MapInitializer() = default;
    virtual void operator()(World& world, Point p, char c) const = 0;
  };
}
