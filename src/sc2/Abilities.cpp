#include "sc2/Abilities.h"

#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/ui.h"

void sc2::Abilities::init(rts::World& w) {
  gather = w.abilities.emplace(std::make_unique<ui::GatherAbility>());
  move = w.abilities.emplace(std::make_unique<ui::MoveAbility>());
}

rts::AbilityId sc2::Abilities::gather;
rts::AbilityId sc2::Abilities::move;
