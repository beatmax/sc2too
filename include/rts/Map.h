#pragma once

#include "types.h"

#include <iosfwd>
#include <utility>
#include <variant>
#include <vector>

namespace rts {

  enum class Blocker { Rock };

  class Map {
  public:
    using Free = std::monostate;
    using Cell = std::variant<Free, Blocker, EntitySPtr>;

    explicit Map(const std::string& s);
    explicit Map(std::istream&& is);
    explicit Map(const std::vector<std::string>& lines);

    const size_t maxX;
    const size_t maxY;

    Cell& at(Position p) { return at(p.x, p.y); }
    Cell& at(size_t x, size_t y) { return cells_[y * maxX + x]; }
    const Cell& at(Position p) const { return at(p.x, p.y); }
    const Cell& at(size_t x, size_t y) const { return cells_[y * maxX + x]; }

    template<typename T>
    void set(Position p, T&& e) {
      cells_[p.y * maxX + p.x] = std::forward<T>(e);
    }

  private:
    std::vector<Cell> cells_;
  };

  inline bool isFree(const Map::Cell& c) { return c.index() == 0; }
  inline bool hasBlocker(const Map::Cell& c) { return c.index() == 1; }
  inline bool hasEntity(const Map::Cell& c) { return c.index() == 2; }

  inline const Blocker& getBlocker(const Map::Cell& c) { return std::get<Blocker>(c); }
  inline EntitySPtr getEntity(Map::Cell& c) { return std::get<EntitySPtr>(c); }
  inline EntitySCPtr getEntity(const Map::Cell& c) { return std::get<EntitySPtr>(c); }
}
