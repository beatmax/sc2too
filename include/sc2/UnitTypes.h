#pragma once

#include "rts/types.h"

#include <map>
#include <string>

namespace sc2 {
  struct UnitTypes {
    static void init(rts::World& w);

    static rts::UnitTypeId assimilator;
    static rts::UnitTypeId cyberCore;
    static rts::UnitTypeId gateway;
    static rts::UnitTypeId immortal;
    static rts::UnitTypeId nexus;
    static rts::UnitTypeId observer;
    static rts::UnitTypeId prism;
    static rts::UnitTypeId probe;
    static rts::UnitTypeId pylon;
    static rts::UnitTypeId robo;
    static rts::UnitTypeId stalker;
    static rts::UnitTypeId templar;
    static rts::UnitTypeId twilight;
    static rts::UnitTypeId zealot;

    static std::map<rts::UnitTypeId, std::string> idToName;
    static std::map<std::string, rts::UnitTypeId> nameToId;
  };
}
