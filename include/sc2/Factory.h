#pragma once

#include "rts/Factory.h"

namespace sc2 {
  struct Factory : public rts::Factory {
    rts::UnitId create(rts::World& w, rts::UnitTypeId t, rts::Point p, rts::SideId sd) final;

    static rts::UnitId nexus(rts::World& w, rts::Point p, rts::SideId sd);
    static rts::UnitId probe(rts::World& w, rts::Point p, rts::SideId sd);
    static rts::ResourceFieldId geyser(rts::World& w, rts::Point p);
    static rts::ResourceFieldId mineralPatch(
        rts::World& w, rts::Point p, rts::ResourceGroupId group);
    static rts::BlockerId rock(rts::World& w, rts::Point p);
  };
}
