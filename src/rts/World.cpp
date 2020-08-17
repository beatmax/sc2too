#include "rts/World.h"

#include "rts/Entity.h"
#include "rts/SemaphoreLock.h"
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
  for (auto& e : entities)
    destroy(e);
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

void rts::World::move(Entity& e, Point p) {
  assert(e.area.size == Vector({1, 1}));  // no need to move big entities so far
  assert(map[p].empty());
  auto& epos = e.area.topLeft;
  map.setContent(epos, Cell::Empty{});
  map.setContent(p, entities.id(e));
  epos = p;
}

void rts::World::destroy(Entity& e) {
  assert(map[e.area.topLeft].contains(Cell::Entity));
  e.onDestroy(*this);
  map.setContent(e.area, Cell::Empty{});
  entities.erase(entities.id(e));
}

void rts::World::destroy(ResourceField& rf) {
  assert(map[rf.area.topLeft].contains(Cell::ResourceField));
  map.setContent(rf.area, Cell::Empty{});
  resourceFields.erase(resourceFields.id(rf));
}

rts::RelativeContent rts::World::relativeContent(SideId side, Point p) const {
  auto& cell{map[p]};
  if (auto* e{entity(cell)})
    return (e->side == side) ? RelativeContent::Friend : RelativeContent::Foe;
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

rts::EntityIdList rts::World::entitiesInArea(
    const Rectangle& area, SideId side, EntityTypeId type) const {
  EntityIdList result;
  forEachPoint(area, [&](Point p) {
    if (auto eId{entityId(p)}) {
      if (std::find(result.begin(), result.end(), eId) == result.end()) {
        const auto& e{entities[eId]};
        if ((!side || e.side == side) && (!type || e.type == type))
          result.push_back(eId);
      }
    }
  });
  return result;
}

const rts::Entity* rts::World::closestEntity(Point p, SideId side, EntityTypeId type) const {
  const Entity* closest{nullptr};
  float closestDistance{std::numeric_limits<float>::infinity()};
  for (const auto& e : entities) {
    if (e.side == side && e.type == type) {
      if (float d{diagonalDistance(p, e.area.center())}; d < closestDistance) {
        closestDistance = d;
        closest = &e;
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
