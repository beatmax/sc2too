#include "sc2/EntityTypes.h"

#include "rts/EntityType.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/Abilities.h"
#include "sc2/constants.h"
#include "sc2/ui.h"

void sc2::EntityTypes::init(rts::World& w) {
  nexus = w.entityTypes.emplace(std::make_unique<ui::NexusType>());
  probe = w.entityTypes.emplace(std::make_unique<ui::ProbeType>());
  {
    auto& probeType{w.entityTypes[probe]};
    probeType.abilities[Abilities::GatherIndex] =
        rts::abilities::gather(Abilities::gather, Abilities::move, nexus, GatherTime, DeliveryTime);
    probeType.abilities[Abilities::MoveIndex] =
        rts::abilities::move(Abilities::move, rts::Speed{4});

    using RC = rts::RelativeContent;
    for (auto rc : {RC::Friend, RC::Foe, RC::Ground})
      probeType.defaultAbility[size_t(rc)] = Abilities::move;
    probeType.defaultAbility[size_t(RC::Resource)] = Abilities::gather;
  }
}

rts::EntityTypeId sc2::EntityTypes::nexus;
rts::EntityTypeId sc2::EntityTypes::probe;
