#pragma once

#include "rts/Blocker.h"
#include "rts/Entity.h"
#include "rts/ResourceField.h"

namespace sc2 {
  struct Factory {
    static rts::Entity nexus(rts::Point p, rts::SideCPtr sd);
    static rts::Entity probe(rts::Point p, rts::SideCPtr sd);
    static rts::ResourceField geyser(rts::Point p);
    static rts::ResourceField mineralPatch(rts::Point p);
    static rts::Blocker rock(rts::Point p);
  };
}
