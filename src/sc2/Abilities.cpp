#include "sc2/Abilities.h"

#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/ui.h"

void sc2::Abilities::init(rts::World& w) {
  chronoBoost = w.createAbility(std::make_unique<ui::ChronoBoostAbility>());
  gather = w.createAbility(std::make_unique<ui::GatherAbility>());
  move = w.createAbility(std::make_unique<ui::MoveAbility>());
  researchCharge = w.createAbility(std::make_unique<ui::ResearchChargeAbility>());
  researchWarpGate = w.createAbility(std::make_unique<ui::ResearchWarpGateAbility>());
  setRallyPoint = w.createAbility(std::make_unique<ui::SetRallyPointAbility>());
  warpInImmortal = w.createAbility(std::make_unique<ui::WarpInImmortalAbility>());
  warpInObserver = w.createAbility(std::make_unique<ui::WarpInObserverAbility>());
  warpInPrism = w.createAbility(std::make_unique<ui::WarpInPrismAbility>());
  warpInProbe = w.createAbility(std::make_unique<ui::WarpInProbeAbility>());
  warpInStalker = w.createAbility(std::make_unique<ui::WarpInStalkerAbility>());
  warpInZealot = w.createAbility(std::make_unique<ui::WarpInZealotAbility>());

  warpInStructure = w.createAbility(std::make_unique<ui::WarpInStructureAbility>());
  warpInAssimilator = w.createAbility(std::make_unique<ui::WarpInAssimilatorAbility>());
  warpInCyberCore = w.createAbility(std::make_unique<ui::WarpInCyberCoreAbility>());
  warpInGateway = w.createAbility(std::make_unique<ui::WarpInGatewayAbility>());
  warpInNexus = w.createAbility(std::make_unique<ui::WarpInNexusAbility>());
  warpInPylon = w.createAbility(std::make_unique<ui::WarpInPylonAbility>());

  warpInStructure2 = w.createAbility(std::make_unique<ui::WarpInStructure2Ability>());
  warpInRobo = w.createAbility(std::make_unique<ui::WarpInRoboAbility>());
  warpInTemplar = w.createAbility(std::make_unique<ui::WarpInTemplarAbility>());
  warpInTwilight = w.createAbility(std::make_unique<ui::WarpInTwilightAbility>());
}

rts::AbilityId sc2::Abilities::chronoBoost;
rts::AbilityId sc2::Abilities::gather;
rts::AbilityId sc2::Abilities::move;
rts::AbilityId sc2::Abilities::researchCharge;
rts::AbilityId sc2::Abilities::researchWarpGate;
rts::AbilityId sc2::Abilities::setRallyPoint;

rts::AbilityId sc2::Abilities::warpInImmortal;
rts::AbilityId sc2::Abilities::warpInObserver;
rts::AbilityId sc2::Abilities::warpInPrism;
rts::AbilityId sc2::Abilities::warpInProbe;
rts::AbilityId sc2::Abilities::warpInStalker;
rts::AbilityId sc2::Abilities::warpInZealot;

rts::AbilityId sc2::Abilities::warpInStructure;
rts::AbilityId sc2::Abilities::warpInAssimilator;
rts::AbilityId sc2::Abilities::warpInCyberCore;
rts::AbilityId sc2::Abilities::warpInGateway;
rts::AbilityId sc2::Abilities::warpInNexus;
rts::AbilityId sc2::Abilities::warpInPylon;

rts::AbilityId sc2::Abilities::warpInStructure2;
rts::AbilityId sc2::Abilities::warpInRobo;
rts::AbilityId sc2::Abilities::warpInTemplar;
rts::AbilityId sc2::Abilities::warpInTwilight;
