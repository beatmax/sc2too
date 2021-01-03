#pragma once

#include "types.h"
#include "util/Matrix.h"

namespace rts {

  class PowerMap {
  public:
    PowerMap() = default;
    PowerMap(const PowerMap&) = delete;
    PowerMap& operator=(const PowerMap&) = delete;

    void initCells(Vector sz) { cells_ = PowerMatrix(sz.y, sz.x); }
    void update(World& w, SideId side, const Circle& c, int16_t inc);
    bool isPowered(Point p) const { return (*this)[p] > 0; }

    Vector size() const { return {cells_.cols(), cells_.rows()}; }
    Rectangle area() const { return Rectangle{{0, 0}, size()}; }

    PowerCount& operator[](Point p) { return (*this)(p.x, p.y); }
    const PowerCount& operator[](Point p) const { return (*this)(p.x, p.y); }
    PowerCount& operator()(Coordinate x, Coordinate y) { return cells_(y, x); }
    const PowerCount& operator()(Coordinate x, Coordinate y) const { return cells_(y, x); }

  private:
    using PowerMatrix = util::Matrix<PowerCount, Coordinate>;

    PowerMatrix cells_;
  };
}
