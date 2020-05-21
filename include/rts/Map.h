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
    using Cell = std::variant<Free, Blocker>;

    void load(std::istream& is);

    size_t maxX{};
    size_t maxY{};

    const Cell& at(size_t x, size_t y) const { return cells_[y * maxX + x]; }

  private:
    template<typename T>
    void set(Location loc, T&& e) {
      cells_[loc.y * maxX + loc.x] = std::forward<T>(e);
    }

    std::vector<Cell> cells_;
  };

  inline bool isFree(const Map::Cell& c) { return c.index() == 0; }
  inline bool hasBlocker(const Map::Cell& c) { return c.index() == 1; }
  inline const Blocker& getBlocker(const Map::Cell& c) { return std::get<Blocker>(c); }
}
