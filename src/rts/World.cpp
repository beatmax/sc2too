#include "rts/World.h"

#include "rts/Entity.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <utility>

rts::World::World(std::vector<Side> s, const MapInitializer& init, std::istream&& is)
  : sides{std::move(s)}, map{*this, init, std::move(is)} {
  assert(sides.size() <= MaxSides);
}

void rts::World::update(const WorldActionList& actions) {
  for (auto& action : actions)
    action(*this);
}

rts::EntityId rts::World::add(Entity&& e) {
  auto id{entities.emplace(std::move(e))};
  map.setContent(entities[id].area, id);
  return id;
}

rts::BlockerId rts::World::add(Blocker&& b) {
  auto id{blockers.emplace(std::move(b))};
  map.setContent(blockers[id].area, id);
  return id;
}

rts::ResourceFieldId rts::World::add(ResourceField&& rf) {
  auto id{resourceFields.emplace(std::move(rf))};
  map.setContent(resourceFields[id].area, id);
  return id;
}

void rts::World::destroy(EntityWId e) {
  if (auto entity = entities[e]) {
    assert(hasEntity(map.at(entity->area.topLeft)));
    map.setContent(entity->area, Map::Free{});
    entities.erase(EntityId(e));
  }
}

void rts::World::move(EntityWId e, Path* path) {
  if (auto entity = entities[e]) {
    assert(entity->area.size == Vector({1, 1}));  // no need to move big entities so far
    assert(!path->empty());
    Point next{path->front()};
    if (isFree(map.at(next))) {
      path->pop_front();
      auto& epos = entity->area.topLeft;
      map.setContent(epos, Map::Free{});
      map.setContent(next, EntityId(e));
      epos = next;
    }
  }
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
