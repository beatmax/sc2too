#pragma once

#include "types.h"
#include "util/Matrix.h"

namespace rts {

  class ProximityMap {
  public:
    ProximityMap() = default;
    ProximityMap(const ProximityMap&) = delete;
    ProximityMap& operator=(const ProximityMap&) = delete;

    void initCells(Vector sz) { cells_ = ProximityMatrix(sz.y, sz.x); }
    PointList update(World& w, const Circle& c, int16_t inc);
    void updateContour(World& w, const Rectangle& r, Coordinate radius, int16_t inc);
    bool isActive(Point p) const { return (*this)[p] > 0; }

    Vector size() const { return {cells_.cols(), cells_.rows()}; }
    Rectangle area() const { return Rectangle{{0, 0}, size()}; }

    ProximityCount& operator[](Point p) { return (*this)(p.x, p.y); }
    const ProximityCount& operator[](Point p) const { return (*this)(p.x, p.y); }
    ProximityCount& operator()(Coordinate x, Coordinate y) { return cells_(y, x); }
    const ProximityCount& operator()(Coordinate x, Coordinate y) const { return cells_(y, x); }

  private:
    using ProximityMatrix = util::Matrix<ProximityCount, Coordinate>;

    ProximityMatrix cells_;
  };
}
