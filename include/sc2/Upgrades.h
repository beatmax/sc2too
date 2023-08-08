#pragma once

#include "rts/types.h"

#include <map>
#include <string>

namespace sc2 {
  struct Upgrades {
    static void init(rts::World& w);

    static rts::UpgradeId charge;
    static rts::UpgradeId warpGate;

    static std::map<rts::UpgradeId, std::string> idToName;
    static std::map<std::string, rts::UpgradeId> nameToId;
  };
}
