#include "sc2/EntityTypes.h"

#include "rts/EntityType.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/Abilities.h"
#include "sc2/Resources.h"
#include "sc2/constants.h"
#include "sc2/ui.h"

void sc2::EntityTypes::init(rts::World& w) {
  nexus = w.entityTypes.emplace(
      rts::ResourceQuantityList{{Resources::mineral(), NexusMineralCost}}, NexusBuildTime,
      std::make_unique<ui::NexusType>());
  probe = w.entityTypes.emplace(
      rts::ResourceQuantityList{{Resources::mineral(), ProbeMineralCost}}, ProbeBuildTime,
      std::make_unique<ui::ProbeType>());
  {
    auto& nexusType{w.entityTypes[nexus]};
    nexusType.addAbility(
        Abilities::WarpInProbeIndex, rts::abilities::Produce{Abilities::warpInProbe, probe});
  }
  {
    auto& probeType{w.entityTypes[probe]};
    probeType.addAbility(
        Abilities::GatherIndex,
        rts::abilities::Gather{Abilities::gather, Abilities::move, nexus, GatherTime,
                               DeliveryTime});
    probeType.addAbility(
        Abilities::MoveIndex, rts::abilities::Move{Abilities::move, rts::Speed{4}});

    using RC = rts::RelativeContent;
    for (auto rc : {RC::Friend, RC::Foe, RC::Ground})
      probeType.defaultAbility[size_t(rc)] = Abilities::move;
    probeType.defaultAbility[size_t(RC::Resource)] = Abilities::gather;
  }
}

rts::EntityTypeId sc2::EntityTypes::nexus;
rts::EntityTypeId sc2::EntityTypes::probe;
