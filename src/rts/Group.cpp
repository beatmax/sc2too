#include "rts/Group.h"

#include "rts/World.h"

#include <tuple>

void rts::Group::set(const World& w, UnitIdList ids) {
  list_.set(w.units, std::move(ids));
}

void rts::Group::add(const World& w, UnitIdList ids) {
  list_.add(w.units, std::move(ids));
}

void rts::Group::remove(const UnitIdList& ids) {
  list_.remove(ids);
}

rts::UnitIdList rts::Group::ids(const World& w) const {
  return list_.lock(w.units);
}

rts::UnitCPtrList rts::Group::items(const World& w) const {
  return list_.items(w.units);
}

bool rts::Group::Compare::operator()(const Unit& u1, const Unit& u2) const {
  const Unit *pu1{&u1}, *pu2{&u2};
  return std::tie(u1.type, pu1) < std::tie(u2.type, pu2);
}
