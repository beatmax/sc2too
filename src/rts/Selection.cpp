#include "rts/Selection.h"

#include "rts/World.h"

void rts::Selection::set(const World& w, EntityIdList ids) {
  list_.set(w.entities, std::move(ids));
}

void rts::Selection::add(const World& w, EntityIdList ids) {
  list_.add(w.entities, std::move(ids));
}

void rts::Selection::remove(const EntityIdList& ids) {
  list_.remove(ids);
}

rts::EntityIdList rts::Selection::ids(const World& w) const {
  return list_.lock(w.entities);
}

rts::EntityCPtrList rts::Selection::items(const World& w) const {
  return list_.items(w.entities);
}
