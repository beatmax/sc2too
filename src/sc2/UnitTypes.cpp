#include "sc2/UnitTypes.h"

#include "rts/UnitType.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/Abilities.h"
#include "sc2/Resources.h"
#include "sc2/Upgrades.h"
#include "sc2/constants.h"
#include "sc2/ui.h"

void sc2::UnitTypes::init(rts::World& w) {
  using RC = rts::RelativeContent;

  // creation order of unit types determines subgroup order in selection

  nexus = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{Resources::mineral, NexusMineralCost}},
      rts::ResourceQuantityList{{Resources::supply, NexusSupplyProvision}}, NexusBuildTime,
      std::make_unique<ui::NexusType>(), NexusMaxEnergy, NexusInitialEnergy);
  gateway = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{Resources::mineral, GatewayMineralCost}},
      rts::ResourceQuantityList{}, GatewayBuildTime, std::make_unique<ui::GatewayType>(), 0, 0,
      nexus, rts::UnitType::RequiresPower::Yes);
  cyberCore = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{Resources::mineral, CyberCoreMineralCost}},
      rts::ResourceQuantityList{}, CyberCoreBuildTime, std::make_unique<ui::CyberCoreType>(), 0, 0,
      gateway, rts::UnitType::RequiresPower::Yes);
  assimilator = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{Resources::mineral, AssimilatorMineralCost}},
      rts::ResourceQuantityList{}, AssimilatorBuildTime, std::make_unique<ui::AssimilatorType>(), 0,
      0, rts::UnitTypeId{}, rts::UnitType::RequiresPower::No, 0, Resources::gas);
  pylon = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{Resources::mineral, PylonMineralCost}},
      rts::ResourceQuantityList{{Resources::supply, PylonSupplyProvision}}, PylonBuildTime,
      std::make_unique<ui::PylonType>(), 0, 0, rts::UnitTypeId{}, rts::UnitType::RequiresPower::No,
      PylonPowerRadius);

  stalker = w.createUnitType(
      rts::UnitType::Kind::Army,
      rts::ResourceQuantityList{
          {Resources::mineral, StalkerMineralCost},
          {Resources::gas, StalkerGasCost},
          {Resources::supply, StalkerSupplyCost}},
      rts::ResourceQuantityList{}, StalkerBuildTime, std::make_unique<ui::StalkerType>(), 0, 0,
      cyberCore);
  zealot = w.createUnitType(
      rts::UnitType::Kind::Army,
      rts::ResourceQuantityList{
          {Resources::mineral, ZealotMineralCost}, {Resources::supply, ZealotSupplyCost}},
      rts::ResourceQuantityList{}, ZealotBuildTime, std::make_unique<ui::ZealotType>());
  probe = w.createUnitType(
      rts::UnitType::Kind::Worker,
      rts::ResourceQuantityList{
          {Resources::mineral, ProbeMineralCost}, {Resources::supply, ProbeSupplyCost}},
      rts::ResourceQuantityList{}, ProbeBuildTime, std::make_unique<ui::ProbeType>());

  {
    auto& cyberCoreType{w.unitTypes[cyberCore]};
    cyberCoreType.addAbility(
        Abilities::ResearchWarpGateIndex,
        rts::abilities::Produce{Abilities::researchWarpGate, Upgrades::warpGate});
  }
  {
    auto& gatewayType{w.unitTypes[gateway]};
    gatewayType.addAbility(
        Abilities::SetRallyPointIndex, rts::abilities::SetRallyPoint{Abilities::setRallyPoint});
    gatewayType.addAbility(
        Abilities::WarpInStalkerIndex, rts::abilities::Produce{Abilities::warpInStalker, stalker});
    gatewayType.addAbility(
        Abilities::WarpInZealotIndex, rts::abilities::Produce{Abilities::warpInZealot, zealot});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      gatewayType.defaultAbility[uint32_t(rc)] = Abilities::SetRallyPointIndex;
  }
  {
    auto& nexusType{w.unitTypes[nexus]};
    nexusType.addAbility(
        Abilities::ChronoBoostIndex,
        rts::abilities::Boost{
            Abilities::chronoBoost, NexusBoostEnergyCost, NexusBoostSpeedUp, NexusBoostDuration});
    nexusType.addAbility(
        Abilities::SetRallyPointIndex, rts::abilities::SetRallyPoint{Abilities::setRallyPoint});
    nexusType.addAbility(
        Abilities::WarpInProbeIndex, rts::abilities::Produce{Abilities::warpInProbe, probe});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      nexusType.defaultAbility[uint32_t(rc)] = Abilities::SetRallyPointIndex;
  }
  {
    auto& probeType{w.unitTypes[probe]};
    probeType.addAbility(
        Abilities::GatherIndex,
        rts::abilities::Gather{Abilities::gather, GatherTime, DeliveryTime});
    probeType.addAbility(Abilities::MoveIndex, rts::abilities::Move{Abilities::move, ProbeSpeed});
    probeType.addAbility(
        Abilities::WarpInStructureIndex,
        rts::abilities::GoToPage{Abilities::warpInStructure, Abilities::WarpInStructurePage});
    probeType.addAbility(
        Abilities::WarpInAssimilatorIndex,
        rts::abilities::Build{Abilities::warpInAssimilator, assimilator});
    probeType.addAbility(
        Abilities::WarpInCyberCoreIndex,
        rts::abilities::Build{Abilities::warpInCyberCore, cyberCore});
    probeType.addAbility(
        Abilities::WarpInGatewayIndex, rts::abilities::Build{Abilities::warpInGateway, gateway});
    probeType.addAbility(
        Abilities::WarpInNexusIndex, rts::abilities::Build{Abilities::warpInNexus, nexus});
    probeType.addAbility(
        Abilities::WarpInPylonIndex, rts::abilities::Build{Abilities::warpInPylon, pylon});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground})
      probeType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
    for (auto rc : {RC::Resource, RC::FriendResource})
      probeType.defaultAbility[uint32_t(rc)] = Abilities::GatherIndex;
  }
  {
    auto& stalkerType{w.unitTypes[stalker]};
    stalkerType.addAbility(
        Abilities::MoveIndex, rts::abilities::Move{Abilities::move, StalkerSpeed});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      stalkerType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
  }
  {
    auto& zealotType{w.unitTypes[zealot]};
    zealotType.addAbility(Abilities::MoveIndex, rts::abilities::Move{Abilities::move, ZealotSpeed});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      zealotType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
  }
}

rts::UnitTypeId sc2::UnitTypes::assimilator;
rts::UnitTypeId sc2::UnitTypes::cyberCore;
rts::UnitTypeId sc2::UnitTypes::gateway;
rts::UnitTypeId sc2::UnitTypes::nexus;
rts::UnitTypeId sc2::UnitTypes::probe;
rts::UnitTypeId sc2::UnitTypes::pylon;
rts::UnitTypeId sc2::UnitTypes::stalker;
rts::UnitTypeId sc2::UnitTypes::zealot;
