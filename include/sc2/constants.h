#pragma once

#include "rts/constants.h"
#include "rts/types.h"

namespace sc2 {
  constexpr rts::GameTime EnergyIncreaseInterval{178};
  constexpr rts::Quantity MaxSupplySlots{200};

  constexpr rts::Quantity GeyserContent{2250};
  constexpr rts::Quantity MineralPatchContent{1500};

  constexpr rts::GameTime GatherTime{3 * rts::GameTimeSecond};
  constexpr rts::GameTime DeliveryTime{5 * rts::GameTimeSecond / 10};

  constexpr rts::GameTime AssimilatorBuildTime{30 * rts::GameTimeSecond};
  constexpr rts::Quantity AssimilatorMineralCost{75};

  constexpr rts::GameTime ChargeUpgradeResearchTime{140 * rts::GameTimeSecond};
  constexpr rts::Quantity ChargeUpgradeMineralCost{100};
  constexpr rts::Quantity ChargeUpgradeGasCost{100};

  constexpr rts::GameTime CyberCoreBuildTime{50 * rts::GameTimeSecond};
  constexpr rts::Quantity CyberCoreMineralCost{150};

  constexpr rts::GameTime GatewayBuildTime{65 * rts::GameTimeSecond};
  constexpr rts::Quantity GatewayMineralCost{150};

  constexpr rts::GameTime ImmortalBuildTime{55 * rts::GameTimeSecond};
  constexpr rts::Quantity ImmortalMineralCost{275};
  constexpr rts::Quantity ImmortalGasCost{100};
  constexpr rts::Quantity ImmortalSupplyCost{4};
  constexpr rts::Speed ImmortalSpeed{2};

  constexpr rts::GameTime NexusBuildTime{100 * rts::GameTimeSecond};
  constexpr rts::Quantity NexusMineralCost{400};
  constexpr rts::Quantity NexusSupplyProvision{15};
  constexpr rts::Quantity NexusMaxEnergy{200};
  constexpr rts::Quantity NexusInitialEnergy{50};
  constexpr rts::Quantity NexusBoostEnergyCost{50};
  constexpr rts::Percent NexusBoostSpeedUp{50};
  constexpr rts::GameTime NexusBoostDuration{28 * rts::GameTimeSecond};

  constexpr rts::GameTime ObserverBuildTime{30 * rts::GameTimeSecond};
  constexpr rts::Quantity ObserverMineralCost{25};
  constexpr rts::Quantity ObserverGasCost{75};
  constexpr rts::Quantity ObserverSupplyCost{1};
  constexpr rts::Speed ObserverSpeed{2};

  constexpr rts::GameTime PrismBuildTime{50 * rts::GameTimeSecond};
  constexpr rts::Quantity PrismMineralCost{250};
  constexpr rts::Quantity PrismSupplyCost{2};
  constexpr rts::Speed PrismSpeed{3};

  constexpr rts::GameTime ProbeBuildTime{17 * rts::GameTimeSecond};
  constexpr rts::Quantity ProbeMineralCost{50};
  constexpr rts::Quantity ProbeSupplyCost{1};
  constexpr rts::Quantity ProbeCargoCapacity{5};
  constexpr rts::Speed ProbeSpeed{3};

  constexpr rts::GameTime PylonBuildTime{25 * rts::GameTimeSecond};
  constexpr rts::Quantity PylonMineralCost{100};
  constexpr rts::Quantity PylonSupplyProvision{8};
  constexpr rts::Coordinate PylonPowerRadius{6};

  constexpr rts::GameTime RoboBuildTime{65 * rts::GameTimeSecond};
  constexpr rts::Quantity RoboMineralCost{150};
  constexpr rts::Quantity RoboGasCost{100};

  constexpr rts::GameTime StalkerBuildTime{42 * rts::GameTimeSecond};
  constexpr rts::Quantity StalkerMineralCost{125};
  constexpr rts::Quantity StalkerGasCost{50};
  constexpr rts::Quantity StalkerSupplyCost{2};
  constexpr rts::Speed StalkerSpeed{3};

  constexpr rts::GameTime TemplarBuildTime{50 * rts::GameTimeSecond};
  constexpr rts::Quantity TemplarMineralCost{150};
  constexpr rts::Quantity TemplarGasCost{200};

  constexpr rts::GameTime TwilightBuildTime{50 * rts::GameTimeSecond};
  constexpr rts::Quantity TwilightMineralCost{150};
  constexpr rts::Quantity TwilightGasCost{100};

  constexpr rts::GameTime WarpGateUpgradeResearchTime{140 * rts::GameTimeSecond};
  constexpr rts::Quantity WarpGateUpgradeMineralCost{50};
  constexpr rts::Quantity WarpGateUpgradeGasCost{50};

  constexpr rts::GameTime ZealotBuildTime{38 * rts::GameTimeSecond};
  constexpr rts::Quantity ZealotMineralCost{100};
  constexpr rts::Quantity ZealotSupplyCost{2};
  constexpr rts::Speed ZealotSpeed{2};
}
