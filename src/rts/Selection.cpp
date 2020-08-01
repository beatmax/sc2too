#include "rts/Selection.h"

#include "rts/World.h"

void rts::Selection::set(const World& world, EntityIdList ids) {
  list_.set(world.entities, std::move(ids));
}

void rts::Selection::add(const World& world, EntityIdList ids) {
  list_.add(world.entities, std::move(ids));
}

void rts::Selection::remove(const EntityIdList& ids) {
  list_.remove(ids);
}

rts::EntityCPtrList rts::Selection::items(const World& world) const {
  return list_.items(world.entities);
}

rts::EntityTypeId rts::Selection::subselectionType(const World& world) const {
  auto entities{items(world)};
  return entities.empty() ? EntityTypeId{} : entities.front()->type;
}
