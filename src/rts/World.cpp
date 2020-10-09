#include "rts/World.h"

#include "rts/SemaphoreLock.h"
#include "rts/Unit.h"
#include "rts/WorldAction.h"
#include "util/algorithm.h"
#include "util/geo.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>

namespace rts {
  namespace {
    template<typename Id>
    WorldObject* objectPtr(World& w, Id id) {
      return &w[id];
    }

    WorldObject* objectPtr(World&, Cell::Empty) { return nullptr; }

    Direction direction(const Vector& v) {
      return v.x < 0
          ? Direction::Left
          : v.x > 0 ? Direction::Right
                    : v.y < 0 ? Direction::Up : v.y > 0 ? Direction::Down : Direction::Right;
    }
  }
}

rts::World::World(FactoryUPtr f) : factory{std::move(f)} {
  factory->init(*this);
}

rts::World::~World() {
  for (auto& u : units)
    destroy(u);
  for (auto& rf : resourceFields)
    destroy(rf);
}

void rts::World::exec(const SideCommand& cmd) {
  update(sides[cmd.side].exec(*this, cmd.command));
}

void rts::World::update(const WorldActionList& actions) {
  for (const auto& action : actions)
    action(*this);
}

rts::UnitId rts::World::add(UnitId id, bool allocCheck) {
  auto& obj{(*this)[id]};
  map.setContent(obj.area, id);
  auto& res{sides[obj.side].resources()};
  const auto& t{unitTypes[obj.type]};
  res.provision(t.provision);
  AllocResult ar{res.allocate(t.cost, AllocFilter::Any, allocCheck)};
  if (ar.result != AllocResult::Success)
    throw std::runtime_error{"adding unit: resource allocation failed (use addForFree()?)"};
  return id;
}

rts::UnitId rts::World::addForFree(UnitId id) {
  auto& obj{(*this)[id]};
  auto& res{sides[obj.side].resources()};
  const auto& cost{unitTypes[obj.type].cost};
  res.provision(cost);
  add(id, false);
  res.deprovision(cost, AllocFilter::Recoverable);
  return id;
}

void rts::World::move(Unit& u, Point p) {
  assert(u.area.size == Vector({1, 1}));  // no need to move big units so far
  assert(map[p].empty());
  auto& epos = u.area.topLeft;
  u.direction = direction(p - epos);
  map.setContent(epos, Cell::Empty{});
  map.setContent(p, id(u));
  epos = p;
}

void rts::World::destroy(Unit& u) {
  assert(map[u.area.topLeft].contains(Cell::Unit));
  u.onDestroy(*this);
  map.setContent(u.area, Cell::Empty{});
  units.erase(id(u));
}

void rts::World::destroy(ResourceField& rf) {
  assert(map[rf.area.topLeft].contains(Cell::ResourceField));
  map.setContent(rf.area, Cell::Empty{});
  resourceFields.erase(id(rf));
}

void rts::World::destroy(ProductionQueue& pq) {
  pq.onDestroy(*this);
  productionQueues.erase(id(pq));
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
  for (Point p : area.points()) {
    if (auto obj{object(p)})
      result.insert(obj);
  }
  return result;
}

rts::UnitIdList rts::World::unitsInArea(const Rectangle& area, SideId side, UnitTypeId type) const {
  UnitIdList result;
  for (Point p : area.points()) {
    if (auto uId{unitId(p)}) {
      if (!util::contains(result, uId)) {
        const auto& u{units[uId]};
        if ((!side || u.side == side) && (!type || u.type == type))
          result.push_back(uId);
      }
    }
  }
  return result;
}

rts::Unit* rts::World::closestUnit(Point p, SideId side, UnitTypeId type) {
  Unit* closest{nullptr};
  float closestDistance{std::numeric_limits<float>::infinity()};
  for (auto& u : units) {
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
  assert(group);
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

const rts::ResourceField* rts::World::closestResourceField(Point p, ResourceId r) const {
  const ResourceField* closest{nullptr};
  float closestDistance{std::numeric_limits<float>::infinity()};
  for (const auto& rf : resourceFields) {
    if (rf.bag.resource == r) {
      if (float d{diagonalDistance(p, rf.area.center())}; d < closestDistance) {
        closestDistance = d;
        closest = &rf;
      }
    }
  }
  return closest;
}

std::optional<rts::Point> rts::World::emptyCellAround(const Rectangle& area, Point towards) const {
  auto points{util::geo::boundingBox(area).outerPoints()};
  auto it{util::minElementBy(points, [&](Point p) {
    return map[p].empty() ? diagonalDistance(p, towards) : std::numeric_limits<float>::max();
  })};
  if (it != points.end() && map[*it].empty())
    return *it;
  return std::nullopt;
}
