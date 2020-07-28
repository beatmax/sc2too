#include "rts/World.h"

#include "rts/Entity.h"
#include "rts/WorldAction.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <utility>

void rts::World::update(const WorldActionList& actions) {
  for (const auto& action : actions)
    std::visit([this](const auto& a) { update(a); }, action);
}

void rts::World::update(const action::AbilityStepAction& action) {
  if (auto entity = entities[action.entityWId]) {
    entity->stepAction(*this, action.abilityId);
  }
}

void rts::World::move(Entity& e, Point p) {
  assert(e.area.size == Vector({1, 1}));  // no need to move big entities so far
  assert(isFree(map.at(p)));
  auto& epos = e.area.topLeft;
  map.setContent(epos, Map::Free{});
  map.setContent(p, entities.id(e));
  epos = p;
}

void rts::World::destroy(const Entity& e) {
  assert(hasEntity(map.at(e.area.topLeft)));
  map.setContent(e.area, Map::Free{});
  entities.erase(entities.id(e));
}

rts::WorldObject& rts::World::operator[](Map::Free) {
  throw std::logic_error{"tried to get object from free cell"};
}

const rts::WorldObject& rts::World::operator[](Map::Free f) const {
  return const_cast<World&>(*this)[f];
}

rts::WorldObject& rts::World::objectAt(Point p) {
  return getObject(*this, map.at(p));
}

const rts::WorldObject& rts::World::objectAt(Point p) const {
  return getObject(*this, map.at(p));
}

rts::WorldObject* rts::World::objectPtrAt(Point p) {
  return getObjectPtr(*this, map.at(p));
}

const rts::WorldObject* rts::World::objectPtrAt(Point p) const {
  return getObjectPtr(*this, map.at(p));
}

rts::Entity& rts::World::entityAt(Point p) {
  assert(hasEntity(map.at(p)));
  return entities[getEntityId(map.at(p))];
}

const rts::Entity& rts::World::entityAt(Point p) const {
  return const_cast<World&>(*this).entityAt(p);
}

rts::Blocker& rts::World::blockerAt(Point p) {
  assert(hasBlocker(map.at(p)));
  return blockers[getBlockerId(map.at(p))];
}

const rts::Blocker& rts::World::blockerAt(Point p) const {
  return const_cast<World&>(*this).blockerAt(p);
}

rts::ResourceField& rts::World::resourceFieldAt(Point p) {
  assert(hasResourceField(map.at(p)));
  return resourceFields[getResourceFieldId(map.at(p))];
}

const rts::ResourceField& rts::World::resourceFieldAt(Point p) const {
  return const_cast<World&>(*this).resourceFieldAt(p);
}
