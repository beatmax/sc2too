#pragma once

#include "rts/types.h"

namespace sc2 {
  struct Factory {
    static rts::EntityId nexus(rts::World& w, rts::Point p, rts::SideId sd);
    static rts::EntityId probe(rts::World& w, rts::Point p, rts::SideId sd);
    static rts::ResourceFieldId geyser(rts::World& w, rts::Point p);
    static rts::ResourceFieldId mineralPatch(
        rts::World& w, rts::Point p, rts::ResourceGroupId group);
    static rts::BlockerId rock(rts::World& w, rts::Point p);
  };
}
