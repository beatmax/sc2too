#pragma once

#include "rts/types.h"

namespace sc2 {
  struct Upgrades {
    static void init(rts::World& w);

    static rts::UpgradeId charge;
    static rts::UpgradeId warpGate;
  };
}
