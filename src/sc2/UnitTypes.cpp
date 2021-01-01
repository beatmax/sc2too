#include "sc2/UnitTypes.h"

#include "rts/UnitType.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/Abilities.h"
#include "sc2/Resources.h"
#include "sc2/constants.h"
#include "sc2/ui.h"

void sc2::UnitTypes::init(rts::World& w) {
  using RC = rts::RelativeContent;

  assimilator = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{Resources::mineral, AssimilatorMineralCost}},
      rts::ResourceQuantityList{}, AssimilatorBuildTime, std::make_unique<ui::AssimilatorType>(),
      Resources::gas);
  gateway = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{Resources::mineral, GatewayMineralCost}},
      rts::ResourceQuantityList{}, GatewayBuildTime, std::make_unique<ui::GatewayType>());
  nexus = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{Resources::mineral, NexusMineralCost}},
      rts::ResourceQuantityList{{Resources::supply, NexusSupplyProvision}}, NexusBuildTime,
      std::make_unique<ui::NexusType>());
  probe = w.createUnitType(
      rts::UnitType::Kind::Worker,
      rts::ResourceQuantityList{
          {Resources::mineral, ProbeMineralCost}, {Resources::supply, ProbeSupplyCost}},
      rts::ResourceQuantityList{}, ProbeBuildTime, std::make_unique<ui::ProbeType>());
  pylon = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{Resources::mineral, PylonMineralCost}},
      rts::ResourceQuantityList{{Resources::supply, PylonSupplyProvision}}, PylonBuildTime,
      std::make_unique<ui::PylonType>());
  zealot = w.createUnitType(
      rts::UnitType::Kind::Army,
      rts::ResourceQuantityList{
          {Resources::mineral, ZealotMineralCost}, {Resources::supply, ZealotSupplyCost}},
      rts::ResourceQuantityList{}, ZealotBuildTime, std::make_unique<ui::ZealotType>());
  {
    auto& gatewayType{w.unitTypes[gateway]};
    gatewayType.addAbility(
        Abilities::SetRallyPointIndex, rts::abilities::SetRallyPoint{Abilities::setRallyPoint});
    gatewayType.addAbility(
        Abilities::WarpInZealotIndex, rts::abilities::Produce{Abilities::warpInZealot, zealot});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      gatewayType.defaultAbility[uint32_t(rc)] = Abilities::SetRallyPointIndex;
  }
  {
    auto& nexusType{w.unitTypes[nexus]};
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
        rts::abilities::Gather{Abilities::gather, nexus, GatherTime, DeliveryTime});
    probeType.addAbility(
        Abilities::MoveIndex, rts::abilities::Move{Abilities::move, rts::Speed{4}});
    probeType.addAbility(
        Abilities::WarpInStructureIndex,
        rts::abilities::GoToPage{Abilities::warpInStructure, Abilities::WarpInStructurePage});
    probeType.addAbility(
        Abilities::WarpInAssimilatorIndex,
        rts::abilities::Build{Abilities::warpInAssimilator, assimilator});
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
    auto& zealotType{w.unitTypes[zealot]};
    zealotType.addAbility(
        Abilities::MoveIndex, rts::abilities::Move{Abilities::move, rts::Speed{4}});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      zealotType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
  }
}

rts::UnitTypeId sc2::UnitTypes::assimilator;
rts::UnitTypeId sc2::UnitTypes::gateway;
rts::UnitTypeId sc2::UnitTypes::nexus;
rts::UnitTypeId sc2::UnitTypes::probe;
rts::UnitTypeId sc2::UnitTypes::pylon;
rts::UnitTypeId sc2::UnitTypes::zealot;
