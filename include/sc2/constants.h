#pragma once

#include "rts/constants.h"
#include "rts/types.h"

namespace sc2 {
  constexpr rts::Quantity MaxSupplySlots{200};

  constexpr rts::Quantity GeyserContent{2250};
  constexpr rts::Quantity MineralPatchContent{1500};

  constexpr rts::GameTime GatherTime{2 * rts::GameTimeSecond};
  constexpr rts::GameTime DeliveryTime{3 * rts::GameTimeSecond / 10};

  constexpr rts::GameTime AssimilatorBuildTime{21 * rts::GameTimeSecond};
  constexpr rts::Quantity AssimilatorMineralCost{75};

  constexpr rts::GameTime GatewayBuildTime{46 * rts::GameTimeSecond};
  constexpr rts::Quantity GatewayMineralCost{150};

  constexpr rts::GameTime NexusBuildTime{71 * rts::GameTimeSecond};
  constexpr rts::Quantity NexusMineralCost{400};
  constexpr rts::Quantity NexusSupplyProvision{15};

  constexpr rts::GameTime ProbeBuildTime{12 * rts::GameTimeSecond};
  constexpr rts::Quantity ProbeMineralCost{50};
  constexpr rts::Quantity ProbeSupplyCost{1};
  constexpr rts::Quantity ProbeCargoCapacity{5};

  constexpr rts::GameTime PylonBuildTime{18 * rts::GameTimeSecond};
  constexpr rts::Quantity PylonMineralCost{100};
  constexpr rts::Quantity PylonSupplyProvision{8};
  constexpr rts::Coordinate PylonPowerRadius{6};

  constexpr rts::GameTime ZealotBuildTime{27 * rts::GameTimeSecond};
  constexpr rts::Quantity ZealotMineralCost{100};
  constexpr rts::Quantity ZealotSupplyCost{2};
}
