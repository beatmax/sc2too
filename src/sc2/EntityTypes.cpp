#include "sc2/EntityTypes.h"

#include "rts/EntityType.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/Abilities.h"
#include "sc2/ui.h"

void sc2::EntityTypes::init(rts::World& w) {
  nexus = w.entityTypes.emplace(std::make_unique<ui::NexusType>());
  probe = w.entityTypes.emplace(std::make_unique<ui::ProbeType>());
  {
    auto& probeType{w.entityTypes[probe]};
    probeType.abilities[0] = rts::abilities::move(Abilities::move, rts::Speed{4});
    probeType.defaultAbility[size_t(rts::RelativeContent::Ground)] = Abilities::move;
  }
}

rts::EntityTypeId sc2::EntityTypes::nexus;
rts::EntityTypeId sc2::EntityTypes::probe;
