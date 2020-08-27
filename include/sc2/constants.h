#pragma once

#include "rts/constants.h"
#include "rts/types.h"

namespace sc2 {
  static constexpr rts::Quantity MaxSupplySlots{200};

  static constexpr rts::Quantity GeyserContent{2250};
  static constexpr rts::Quantity MineralPatchContent{1800};

  static constexpr rts::GameTime GatherTime{2 * rts::GameTimeSecond};
  static constexpr rts::GameTime DeliveryTime{3 * rts::GameTimeSecond / 10};

  static constexpr rts::GameTime NexusBuildTime{71 * rts::GameTimeSecond};
  static constexpr rts::Quantity NexusMineralCost{400};
  static constexpr rts::Quantity NexusSupplyProvision{15};

  static constexpr rts::GameTime ProbeBuildTime{12 * rts::GameTimeSecond};
  static constexpr rts::Quantity ProbeMineralCost{50};
  static constexpr rts::Quantity ProbeSupplyCost{1};
  static constexpr rts::Quantity ProbeCargoCapacity{5};
}
