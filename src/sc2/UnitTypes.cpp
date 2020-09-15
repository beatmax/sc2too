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

  nexus = w.createUnitType(
      rts::ResourceQuantityList{{Resources::mineral, NexusMineralCost}},
      rts::ResourceQuantityList{{Resources::supply, NexusSupplyProvision}}, NexusBuildTime,
      std::make_unique<ui::NexusType>());
  probe = w.createUnitType(
      rts::ResourceQuantityList{
          {Resources::mineral, ProbeMineralCost}, {Resources::supply, ProbeSupplyCost}},
      rts::ResourceQuantityList{}, ProbeBuildTime, std::make_unique<ui::ProbeType>());
  {
    auto& nexusType{w.unitTypes[nexus]};
    nexusType.addAbility(
        Abilities::SetRallyPointIndex, rts::abilities::SetRallyPoint{Abilities::setRallyPoint});
    nexusType.addAbility(
        Abilities::WarpInProbeIndex, rts::abilities::Produce{Abilities::warpInProbe, probe});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground, RC::Resource})
      nexusType.defaultAbility[uint32_t(rc)] = Abilities::setRallyPoint;
  }
  {
    auto& probeType{w.unitTypes[probe]};
    probeType.addAbility(
        Abilities::GatherIndex,
        rts::abilities::Gather{
            Abilities::gather, Abilities::move, nexus, GatherTime, DeliveryTime});
    probeType.addAbility(
        Abilities::MoveIndex, rts::abilities::Move{Abilities::move, rts::Speed{4}});

    for (auto rc : {RC::Friend, RC::Foe, RC::Ground})
      probeType.defaultAbility[uint32_t(rc)] = Abilities::move;
    probeType.defaultAbility[uint32_t(RC::Resource)] = Abilities::gather;
  }
}

rts::UnitTypeId sc2::UnitTypes::nexus;
rts::UnitTypeId sc2::UnitTypes::probe;
