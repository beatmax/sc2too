#pragma once

#include "Blocker.h"
#include "types.h"
#include "util/Matrix.h"

#include <iosfwd>
#include <utility>
#include <variant>
#include <vector>

namespace rts {

  class CellCreator;

  class Map {
  public:
    using Free = std::monostate;
    using Cell = std::variant<Free, Blocker, EntitySPtr>;

    explicit Map(const CellCreator& creator, const std::string& s);
    explicit Map(const CellCreator& creator, std::istream&& is);
    explicit Map(const CellCreator& creator, const std::vector<std::string>& lines);

    const Coordinate maxX;
    const Coordinate maxY;

    Cell& at(Position p) { return at(p.x, p.y); }
    Cell& at(Coordinate x, Coordinate y) { return cells_(y, x); }
    const Cell& at(Position p) const { return at(p.x, p.y); }
    const Cell& at(Coordinate x, Coordinate y) const { return cells_(y, x); }

    template<typename T>
    void set(Position p, T&& e) {
      cells_(p.y, p.x) = std::forward<T>(e);
    }

  private:
    util::Matrix<Cell, Coordinate> cells_;
  };

  inline bool isFree(const Map::Cell& c) { return c.index() == 0; }
  inline bool hasBlocker(const Map::Cell& c) { return c.index() == 1; }
  inline bool hasEntity(const Map::Cell& c) { return c.index() == 2; }

  inline const Blocker& getBlocker(const Map::Cell& c) { return std::get<Blocker>(c); }
  inline EntitySPtr getEntity(Map::Cell& c) { return std::get<EntitySPtr>(c); }
  inline EntitySCPtr getEntity(const Map::Cell& c) { return std::get<EntitySPtr>(c); }

  class CellCreator {
  public:
    virtual ~CellCreator() = default;
    virtual Map::Cell operator()(char c) const = 0;
  };
}
