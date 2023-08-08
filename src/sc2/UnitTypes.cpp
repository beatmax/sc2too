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
  robo = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{
          {Resources::mineral, RoboMineralCost}, {Resources::gas, RoboGasCost}},
      rts::ResourceQuantityList{}, RoboBuildTime, std::make_unique<ui::RoboType>(), 0, 0, cyberCore,
      rts::UnitType::RequiresPower::Yes);
  twilight = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{
          {Resources::mineral, TwilightMineralCost}, {Resources::gas, TwilightGasCost}},
      rts::ResourceQuantityList{}, TwilightBuildTime, std::make_unique<ui::TwilightType>(), 0, 0,
      cyberCore, rts::UnitType::RequiresPower::Yes);
  templar = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{
          {Resources::mineral, TemplarMineralCost}, {Resources::gas, TemplarGasCost}},
      rts::ResourceQuantityList{}, TemplarBuildTime, std::make_unique<ui::TemplarType>(), 0, 0,
      twilight, rts::UnitType::RequiresPower::Yes);
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

  prism = w.createUnitType(
      rts::UnitType::Kind::Army,
      rts::ResourceQuantityList{
          {Resources::mineral, PrismMineralCost}, {Resources::supply, PrismSupplyCost}},
      rts::ResourceQuantityList{}, PrismBuildTime, std::make_unique<ui::PrismType>());
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
  immortal = w.createUnitType(
      rts::UnitType::Kind::Army,
      rts::ResourceQuantityList{
          {Resources::mineral, ImmortalMineralCost},
          {Resources::gas, ImmortalGasCost},
          {Resources::supply, ImmortalSupplyCost}},
      rts::ResourceQuantityList{}, ImmortalBuildTime, std::make_unique<ui::ImmortalType>());
  observer = w.createUnitType(
      rts::UnitType::Kind::Army,
      rts::ResourceQuantityList{
          {Resources::mineral, ObserverMineralCost},
          {Resources::gas, ObserverGasCost},
          {Resources::supply, ObserverSupplyCost}},
      rts::ResourceQuantityList{}, ObserverBuildTime, std::make_unique<ui::ObserverType>());
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
    auto& immortalType{w.unitTypes[immortal]};
    immortalType.addAbility(
        Abilities::MoveIndex, rts::abilities::Move{Abilities::move, ImmortalSpeed});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      immortalType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
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
    auto& observerType{w.unitTypes[observer]};
    observerType.addAbility(
        Abilities::MoveIndex, rts::abilities::Move{Abilities::move, ObserverSpeed});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      observerType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
  }
  {
    auto& prismType{w.unitTypes[prism]};
    prismType.addAbility(Abilities::MoveIndex, rts::abilities::Move{Abilities::move, PrismSpeed});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      prismType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
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

    probeType.addAbility(
        Abilities::WarpInStructure2Index,
        rts::abilities::GoToPage{Abilities::warpInStructure2, Abilities::WarpInStructure2Page});
    probeType.addAbility(
        Abilities::WarpInRoboIndex, rts::abilities::Build{Abilities::warpInRobo, robo});
    probeType.addAbility(
        Abilities::WarpInTemplarIndex, rts::abilities::Build{Abilities::warpInTemplar, templar});
    probeType.addAbility(
        Abilities::WarpInTwilightIndex, rts::abilities::Build{Abilities::warpInTwilight, twilight});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground})
      probeType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
    for (auto rc : {RC::Resource, RC::FriendResource})
      probeType.defaultAbility[uint32_t(rc)] = Abilities::GatherIndex;
  }
  {
    auto& roboType{w.unitTypes[robo]};
    roboType.addAbility(
        Abilities::SetRallyPointIndex, rts::abilities::SetRallyPoint{Abilities::setRallyPoint});
    roboType.addAbility(
        Abilities::WarpInImmortalIndex,
        rts::abilities::Produce{Abilities::warpInImmortal, immortal});
    roboType.addAbility(
        Abilities::WarpInObserverIndex,
        rts::abilities::Produce{Abilities::warpInObserver, observer});
    roboType.addAbility(
        Abilities::WarpInPrismIndex, rts::abilities::Produce{Abilities::warpInPrism, prism});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      roboType.defaultAbility[uint32_t(rc)] = Abilities::SetRallyPointIndex;
  }
  {
    auto& stalkerType{w.unitTypes[stalker]};
    stalkerType.addAbility(
        Abilities::MoveIndex, rts::abilities::Move{Abilities::move, StalkerSpeed});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      stalkerType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
  }
  {
    auto& twilightType{w.unitTypes[twilight]};
    twilightType.addAbility(
        Abilities::ResearchChargeIndex,
        rts::abilities::Produce{Abilities::researchCharge, Upgrades::charge});
  }
  {
    auto& zealotType{w.unitTypes[zealot]};
    zealotType.addAbility(Abilities::MoveIndex, rts::abilities::Move{Abilities::move, ZealotSpeed});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource, RC::FriendResource})
      zealotType.defaultAbility[uint32_t(rc)] = Abilities::MoveIndex;
  }

  auto setName = [](rts::UnitTypeId id, std::string name) {
    idToName[id] = name;
    nameToId[name] = id;
  };
  setName(assimilator, "assimilator");
  setName(cyberCore, "cyber_core");
  setName(gateway, "gateway");
  setName(immortal, "immortal");
  setName(nexus, "nexus");
  setName(observer, "observer");
  setName(prism, "prism");
  setName(probe, "probe");
  setName(pylon, "pylon");
  setName(robo, "robo");
  setName(stalker, "stalker");
  setName(templar, "templar");
  setName(twilight, "twilight");
  setName(zealot, "zealot");
}

rts::UnitTypeId sc2::UnitTypes::assimilator;
rts::UnitTypeId sc2::UnitTypes::cyberCore;
rts::UnitTypeId sc2::UnitTypes::gateway;
rts::UnitTypeId sc2::UnitTypes::immortal;
rts::UnitTypeId sc2::UnitTypes::nexus;
rts::UnitTypeId sc2::UnitTypes::observer;
rts::UnitTypeId sc2::UnitTypes::prism;
rts::UnitTypeId sc2::UnitTypes::probe;
rts::UnitTypeId sc2::UnitTypes::pylon;
rts::UnitTypeId sc2::UnitTypes::robo;
rts::UnitTypeId sc2::UnitTypes::stalker;
rts::UnitTypeId sc2::UnitTypes::templar;
rts::UnitTypeId sc2::UnitTypes::twilight;
rts::UnitTypeId sc2::UnitTypes::zealot;

std::map<rts::UnitTypeId, std::string> sc2::UnitTypes::idToName;
std::map<std::string, rts::UnitTypeId> sc2::UnitTypes::nameToId;
