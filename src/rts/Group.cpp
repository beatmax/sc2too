#include "rts/Group.h"

#include "rts/World.h"

#include <tuple>

void rts::Group::set(const World& w, EntityIdList ids) {
  list_.set(w.entities, std::move(ids));
}

void rts::Group::add(const World& w, EntityIdList ids) {
  list_.add(w.entities, std::move(ids));
}

void rts::Group::remove(const EntityIdList& ids) {
  list_.remove(ids);
}

rts::EntityIdList rts::Group::ids(const World& w) const {
  return list_.lock(w.entities);
}

rts::EntityCPtrList rts::Group::items(const World& w) const {
  return list_.items(w.entities);
}

bool rts::Group::Compare::operator()(const Entity& e1, const Entity& e2) const {
  const Entity *pe1{&e1}, *pe2{&e2};
  return std::tie(e1.type, pe1) < std::tie(e2.type, pe2);
}
