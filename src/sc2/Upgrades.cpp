#include "sc2/Upgrades.h"

#include "rts/Upgrade.h"
#include "rts/World.h"
#include "sc2/Resources.h"
#include "sc2/constants.h"
#include "sc2/ui.h"

void sc2::Upgrades::init(rts::World& w) {
  charge = w.createUpgrade(
      rts::ResourceQuantityList{
          {Resources::mineral, ChargeUpgradeMineralCost}, {Resources::gas, ChargeUpgradeGasCost}},
      ChargeUpgradeResearchTime, std::make_unique<ui::ChargeUpgrade>());
  warpGate = w.createUpgrade(
      rts::ResourceQuantityList{
          {Resources::mineral, WarpGateUpgradeMineralCost},
          {Resources::gas, WarpGateUpgradeGasCost}},
      WarpGateUpgradeResearchTime, std::make_unique<ui::WarpGateUpgrade>());
}

rts::UpgradeId sc2::Upgrades::charge;
rts::UpgradeId sc2::Upgrades::warpGate;
