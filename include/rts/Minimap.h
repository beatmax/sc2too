#pragma once

#include "constants.h"
#include "types.h"
#include "util/Matrix.h"

namespace rts {

  struct MinimapCell {
    IdIndexedArray<SideId, uint16_t, MaxSides> sideUnits;
    uint16_t units;
    uint16_t resourceFields{};
    uint16_t blockers{};
  };

  class Minimap {
  public:
    Minimap() = default;
    Minimap(const Minimap&) = delete;
    Minimap& operator=(const Minimap&) = delete;

    void init(Vector sz) { cells_ = Matrix(sz.y, sz.x); }
    void initMapCells(Map& map);

    void update(const Map& map, const Unit& u, int16_t inc);
    void update(const Map& map, const ResourceField& rf, int16_t inc);
    void update(const Map& map, const Blocker& b, int16_t inc);

    Vector size() const { return {cells_.cols(), cells_.rows()}; }
    Rectangle rect() const { return Rectangle{{0, 0}, size()}; }
    Point mapToMinimap(Point p) const { return transform(p, factor_); }
    Point minimapToMap(Point p) const { return transform(p, inverseFactor_); }
    Vector minimapToMap(FVector v) const { return round(scale(v, inverseFactor_)); }
    const FVector& factor() const { return factor_; }
    const Vector approxCellSize() const { return round(inverseFactor_); }

    MinimapCell& operator[](MinimapCellId idx) { return cells_.data(idx); }
    const MinimapCell& operator[](MinimapCellId idx) const { return cells_.data(idx); }
    MinimapCell& operator[](Point p) { return (*this)(p.x, p.y); }
    const MinimapCell& operator[](Point p) const { return (*this)(p.x, p.y); }
    MinimapCell& operator()(Coordinate x, Coordinate y) { return cells_(y, x); }
    const MinimapCell& operator()(Coordinate x, Coordinate y) const { return cells_(y, x); }

  private:
    using Matrix = util::Matrix<MinimapCell, Coordinate>;

    Matrix cells_{1, 1};
    FVector factor_{};
    FVector inverseFactor_{};
  };
}
