#include "rts/World.h"

#include "rts/SemaphoreLock.h"
#include "rts/Unit.h"
#include "rts/WorldAction.h"
#include "util/geo.h"

#include <algorithm>
#include <cassert>
#include <limits>

namespace rts {
  namespace {
    template<typename Id>
    WorldObject* objectPtr(World& w, Id id) {
      return &w[id];
    }

    WorldObject* objectPtr(World&, Cell::Empty) { return nullptr; }
  }
}

rts::World::~World() {
  for (auto& u : units)
    destroy(u);
  for (auto& rf : resourceFields)
    destroy(rf);
}

void rts::World::exec(const SideCommandList& commands) {
  for (const auto& cmd : commands)
    update(sides[cmd.side].exec(*this, cmd.command));
}

void rts::World::update(const WorldActionList& actions) {
  for (const auto& action : actions)
    action(*this);
}

void rts::World::move(Unit& u, Point p) {
  assert(u.area.size == Vector({1, 1}));  // no need to move big units so far
  assert(map[p].empty());
  auto& epos = u.area.topLeft;
  map.setContent(epos, Cell::Empty{});
  map.setContent(p, units.id(u));
  epos = p;
}

void rts::World::destroy(Unit& u) {
  assert(map[u.area.topLeft].contains(Cell::Unit));
  u.onDestroy(*this);
  map.setContent(u.area, Cell::Empty{});
  units.erase(units.id(u));
}

void rts::World::destroy(ResourceField& rf) {
  assert(map[rf.area.topLeft].contains(Cell::ResourceField));
  map.setContent(rf.area, Cell::Empty{});
  resourceFields.erase(resourceFields.id(rf));
}

rts::RelativeContent rts::World::relativeContent(SideId side, Point p) const {
  auto& cell{map[p]};
  if (auto* u{unit(cell)})
    return (u->side == side) ? RelativeContent::Friend : RelativeContent::Foe;
  else if (cell.contains(Cell::ResourceField))
    return RelativeContent::Resource;
  return RelativeContent::Ground;
}

rts::WorldObject* rts::World::object(const Cell& c) {
  return std::visit([this](auto id) { return objectPtr(*this, id); }, c.content);
}

std::set<rts::WorldObjectCPtr> rts::World::objectsInArea(const Rectangle& area) const {
  std::set<WorldObjectCPtr> result;
  forEachPoint(area, [&](Point p) {
    if (auto obj{object(p)})
      result.insert(obj);
  });
  return result;
}

rts::UnitIdList rts::World::unitsInArea(const Rectangle& area, SideId side, UnitTypeId type) const {
  UnitIdList result;
  forEachPoint(area, [&](Point p) {
    if (auto uId{unitId(p)}) {
      if (std::find(result.begin(), result.end(), uId) == result.end()) {
        const auto& u{units[uId]};
        if ((!side || u.side == side) && (!type || u.type == type))
          result.push_back(uId);
      }
    }
  });
  return result;
}

const rts::Unit* rts::World::closestUnit(Point p, SideId side, UnitTypeId type) const {
  const Unit* closest{nullptr};
  float closestDistance{std::numeric_limits<float>::infinity()};
  for (const auto& u : units) {
    if (u.side == side && u.type == type) {
      if (float d{diagonalDistance(p, u.area.center())}; d < closestDistance) {
        closestDistance = d;
        closest = &u;
      }
    }
  }
  return closest;
}

const rts::ResourceField* rts::World::closestResourceField(
    Point p, ResourceGroupId group, bool blockedOk) const {
  const ResourceField* closest{nullptr};
  float closestDistance{std::numeric_limits<float>::infinity()};
  for (const auto& rf : resourceFields) {
    if (rf.group == group && (blockedOk || !rf.sem.blocked())) {
      if (float d{diagonalDistance(p, rf.area.center())}; d < closestDistance) {
        closestDistance = d;
        closest = &rf;
      }
    }
  }
  return closest;
}

std::optional<rts::Point> rts::World::emptyCellAround(const Rectangle& area) const {
  return findInOuterPoints(util::geo::boundingBox(area), [&](Point p) { return map[p].empty(); });
}
