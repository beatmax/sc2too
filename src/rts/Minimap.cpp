#include "rts/Minimap.h"

#include "rts/Blocker.h"
#include "rts/Map.h"
#include "rts/ResourceField.h"
#include "rts/Unit.h"

void rts::Minimap::initMapCells(Map& map) {
  factor_ = relation(size(), map.size());
  inverseFactor_ = 1.f / factor_;
  for (auto p : map.area().points()) {
    auto mp{mapToMinimap(p)};
    MinimapCellId cellId(mp.y * cells_.cols() + mp.x);
    assert(cellId < cells_.size());
    map[p].minimapCell = cellId;
  }
}

void rts::Minimap::update(const Map& map, const Unit& u, int16_t inc) {
  for (auto p : u.area.points()) {
    auto& c{cells_.data(map[p].minimapCell)};
    c.sideUnits[u.side] += inc;
    c.units += inc;
  }
}

void rts::Minimap::update(const Map& map, const ResourceField& rf, int16_t inc) {
  for (auto p : rf.area.points())
    cells_.data(map[p].minimapCell).resourceFields += inc;
}

void rts::Minimap::update(const Map& map, const Blocker& b, int16_t inc) {
  for (auto p : b.area.points())
    cells_.data(map[p].minimapCell).blockers += inc;
}
