#pragma once

#include "Cell.h"
#include "types.h"
#include "util/Matrix.h"

#include <iosfwd>
#include <utility>
#include <vector>

namespace rts {

  class MapInitializer;
  class World;

  class Map {
  public:
    Map() = default;
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    void load(World& w, const MapInitializer& init, const std::string& fpath);
    void load(World& w, const MapInitializer& init, std::istream&& is);
    void load(World& w, const MapInitializer& init, const std::vector<std::string>& lines);
    void initCells(Vector sz) { cells_ = CellMatrix(sz.y, sz.x); }

    Vector size() const { return {cells_.cols(), cells_.rows()}; }
    Rectangle area() const { return Rectangle{{0, 0}, size()}; }
    Point center() const { return area().center(); }
    size_t cellCount() const { return cells_.size(); }
    bool isEmpty(const Rectangle& area) const;

    Cell& operator[](Point p) { return (*this)(p.x, p.y); }
    const Cell& operator[](Point p) const { return (*this)(p.x, p.y); }
    Cell& operator()(Coordinate x, Coordinate y) { return cells_(y, x); }
    const Cell& operator()(Coordinate x, Coordinate y) const { return cells_(y, x); }

    template<typename T>
    void setContent(Point p, T&& o) {
      cells_(p.y, p.x).content = std::forward<T>(o);
    }

    template<typename T>
    void setContent(const Rectangle& area, const T& o) {
      for (Point p : area.points())
        cells_(p.y, p.x).content = o;
    }

  private:
    using CellMatrix = util::Matrix<Cell, Coordinate>;

    CellMatrix cells_;
  };

  class MapInitializer {
  public:
    virtual ~MapInitializer() = default;
    virtual Cell::Content operator()(
        World& w, Point p, char c, const std::vector<std::string>& lines) const = 0;
  };
}
