#include "rts/World.h"

#include "rts/Entity.h"
#include "rts/WorldAction.h"

#include <algorithm>
#include <cassert>

namespace rts {
  namespace {
    template<typename Id>
    WorldObject* objectPtr(World& w, Id id) {
      return &w[id];
    }

    WorldObject* objectPtr(World&, Cell::Empty) { return nullptr; }
  }
}

void rts::World::update(const WorldActionList& actions) {
  for (const auto& action : actions)
    std::visit([this](const auto& a) { update(a); }, action);
}

void rts::World::move(Entity& e, Point p) {
  assert(e.area.size == Vector({1, 1}));  // no need to move big entities so far
  assert(map[p].empty());
  auto& epos = e.area.topLeft;
  map.setContent(epos, Cell::Empty{});
  map.setContent(p, entities.id(e));
  epos = p;
}

void rts::World::destroy(const Entity& e) {
  assert(map[e.area.topLeft].contains(Cell::Entity));
  map.setContent(e.area, Cell::Empty{});
  entities.erase(entities.id(e));
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
      const auto& e{entities[eId]};
      if ((!side || e.side == side) && (!type || e.type == type))
        result.push_back(eId);
    }
  });
  return result;
}

void rts::World::update(const action::CommandAction& action) {
  sides[action.sideId].exec(*this, action.command);
}

void rts::World::update(const action::AbilityStepAction& action) {
  if (auto entity = entities[action.entityWId]) {
    entity->stepAction(*this, action.abilityIndex);
  }
}
