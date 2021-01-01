#include "sc2/Abilities.h"

#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/ui.h"

void sc2::Abilities::init(rts::World& w) {
  gather = w.createAbility(std::make_unique<ui::GatherAbility>());
  move = w.createAbility(std::make_unique<ui::MoveAbility>());
  setRallyPoint = w.createAbility(std::make_unique<ui::SetRallyPointAbility>());
  warpInProbe = w.createAbility(std::make_unique<ui::WarpInProbeAbility>());
  warpInZealot = w.createAbility(std::make_unique<ui::WarpInZealotAbility>());

  warpInStructure = w.createAbility(std::make_unique<ui::WarpInStructureAbility>());
  warpInAssimilator = w.createAbility(std::make_unique<ui::WarpInAssimilatorAbility>());
  warpInGateway = w.createAbility(std::make_unique<ui::WarpInGatewayAbility>());
  warpInNexus = w.createAbility(std::make_unique<ui::WarpInNexusAbility>());
  warpInPylon = w.createAbility(std::make_unique<ui::WarpInPylonAbility>());
}

rts::AbilityId sc2::Abilities::gather;
rts::AbilityId sc2::Abilities::move;
rts::AbilityId sc2::Abilities::setRallyPoint;
rts::AbilityId sc2::Abilities::warpInProbe;
rts::AbilityId sc2::Abilities::warpInZealot;

rts::AbilityId sc2::Abilities::warpInStructure;
rts::AbilityId sc2::Abilities::warpInAssimilator;
rts::AbilityId sc2::Abilities::warpInGateway;
rts::AbilityId sc2::Abilities::warpInNexus;
rts::AbilityId sc2::Abilities::warpInPylon;
