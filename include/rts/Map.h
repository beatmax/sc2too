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

  class CellCreator;
  class World;

  class Map {
  public:
    using Free = std::monostate;
    using Cell = std::variant<Free, WorldObjectSPtr>;

    explicit Map(World& world, const CellCreator& creator, std::istream&& is);
    explicit Map(World& world, const CellCreator& creator, const std::vector<std::string>& lines);

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

  inline bool isFree(const Map::Cell& c) { return c.index() == 0; }
  inline bool hasWorldObject(const Map::Cell& c) { return c.index() == 1; }
  inline bool has(const Map::Cell& c, WorldObject::Type objectType) {
    return hasWorldObject(c) && std::get<WorldObjectSPtr>(c)->type == objectType;
  }
  inline bool hasBlocker(const Map::Cell& c) { return has(c, WorldObject::Type::Blocker); }
  inline bool hasEntity(const Map::Cell& c) { return has(c, WorldObject::Type::Entity); }
  inline bool hasResourceField(const Map::Cell& c) {
    return has(c, WorldObject::Type::ResourceField);
  }

  namespace detail {
    inline WorldObjectSPtr getWorldObject(Map::Cell& c) {
      assert(hasWorldObject(c));
      return std::get<WorldObjectSPtr>(c);
    }
    inline WorldObjectSCPtr getWorldObject(const Map::Cell& c) {
      assert(hasWorldObject(c));
      return std::get<WorldObjectSPtr>(c);
    }
  }

  inline const WorldObject& getWorldObject(const Map::Cell& c) {
    return *detail::getWorldObject(c);
  }

  template<typename T>
  inline std::shared_ptr<T> get(Map::Cell& c) {
    assert(has(c, T::worldObjectType));
    return std::static_pointer_cast<T>(detail::getWorldObject(c));
  }
  template<typename T>
  inline std::shared_ptr<const T> get(const Map::Cell& c) {
    assert(has(c, T::worldObjectType));
    return std::static_pointer_cast<const T>(detail::getWorldObject(c));
  }

  inline const Blocker& getBlocker(const Map::Cell& c) { return *get<Blocker>(c); }

  inline ResourceFieldSPtr getResourceField(Map::Cell& c) { return get<ResourceField>(c); }
  inline ResourceFieldSCPtr getResourceField(const Map::Cell& c) { return get<ResourceField>(c); }

  inline EntitySPtr getEntity(Map::Cell& c) { return get<Entity>(c); }
  inline EntitySCPtr getEntity(const Map::Cell& c) { return get<Entity>(c); }

  class CellCreator {
  public:
    virtual ~CellCreator() = default;
    virtual Map::Cell operator()(const World& world, Point p, char c) const = 0;
  };
}
