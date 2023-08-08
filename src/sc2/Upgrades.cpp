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

  auto setName = [](rts::UpgradeId id, std::string name) {
    idToName[id] = name;
    nameToId[name] = id;
  };
  setName(charge, "charge");
  setName(warpGate, "warp_gate");
}

rts::UpgradeId sc2::Upgrades::charge;
rts::UpgradeId sc2::Upgrades::warpGate;

std::map<rts::UpgradeId, std::string> sc2::Upgrades::idToName;
std::map<std::string, rts::UpgradeId> sc2::Upgrades::nameToId;
