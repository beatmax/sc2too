#include "sc2/Factory.h"

#include "rts/World.h"
#include "sc2/Abilities.h"
#include "sc2/BuildOrder.h"
#include "sc2/Resources.h"
#include "sc2/UnitTypes.h"
#include "sc2/Upgrades.h"
#include "sc2/constants.h"
#include "sc2/ui.h"
#include "ui/SideUi.h"

namespace {
  sc2::BuildOrder buildOrder;
}

void sc2::Factory::init(rts::World& w) {
  Resources::init(w);
  Abilities::init(w);
  Upgrades::init(w);
  UnitTypes::init(w);
  buildOrder.init(w);
}

void sc2::Factory::onProduce(rts::World& w, rts::ProducibleId p, rts::SideId sd) {
  buildOrder.check(w, p, sd);
}

rts::UnitId sc2::Factory::create(rts::World& w, rts::UnitTypeId t, rts::SideId sd) {
  if (t == UnitTypes::assimilator)
    return assimilator(w, sd);
  if (t == UnitTypes::cyberCore)
    return cyberCore(w, sd);
  if (t == UnitTypes::gateway)
    return gateway(w, sd);
  if (t == UnitTypes::immortal)
    return immortal(w, sd);
  if (t == UnitTypes::nexus)
    return nexus(w, sd);
  if (t == UnitTypes::observer)
    return observer(w, sd);
  if (t == UnitTypes::prism)
    return prism(w, sd);
  if (t == UnitTypes::probe)
    return probe(w, sd);
  if (t == UnitTypes::pylon)
    return pylon(w, sd);
  if (t == UnitTypes::robo)
    return robo(w, sd);
  if (t == UnitTypes::stalker)
    return stalker(w, sd);
  if (t == UnitTypes::templar)
    return templar(w, sd);
  if (t == UnitTypes::twilight)
    return twilight(w, sd);
  if (t == UnitTypes::zealot)
    return zealot(w, sd);
  return {};
}

rts::UnitId sc2::Factory::assimilator(rts::World& w, rts::SideId sd, rts::ResourceFieldId rf) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{3, 3}, UnitTypes::assimilator, sd, std::make_unique<ui::Assimilator>(sideColor),
      0, rts::ProductionQueueId{}, rf);
}

rts::UnitId sc2::Factory::cyberCore(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{3, 3}, UnitTypes::cyberCore, sd, std::make_unique<ui::CyberCore>(sideColor), 0,
      w.createProductionQueue(sd));
}

rts::UnitId sc2::Factory::gateway(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{3, 3}, UnitTypes::gateway, sd, std::make_unique<ui::Gateway>(sideColor), 0,
      w.createProductionQueue(sd));
}

rts::UnitId sc2::Factory::immortal(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{1, 1}, UnitTypes::immortal, sd, std::make_unique<ui::Immortal>(sideColor));
}

rts::UnitId sc2::Factory::nexus(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{5, 5}, UnitTypes::nexus, sd, std::make_unique<ui::Nexus>(sideColor), 0,
      w.createProductionQueue(sd));
}

rts::UnitId sc2::Factory::observer(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{1, 1}, UnitTypes::observer, sd, std::make_unique<ui::Observer>(sideColor));
}

rts::UnitId sc2::Factory::prism(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{1, 1}, UnitTypes::prism, sd, std::make_unique<ui::Prism>(sideColor));
}

rts::UnitId sc2::Factory::probe(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{1, 1}, UnitTypes::probe, sd, std::make_unique<ui::Probe>(sideColor),
      ProbeCargoCapacity);
}

rts::UnitId sc2::Factory::pylon(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{2, 2}, UnitTypes::pylon, sd, std::make_unique<ui::Pylon>(sideColor));
}

rts::UnitId sc2::Factory::robo(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{3, 3}, UnitTypes::robo, sd, std::make_unique<ui::Robo>(sideColor), 0,
      w.createProductionQueue(sd));
}

rts::UnitId sc2::Factory::stalker(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{1, 1}, UnitTypes::stalker, sd, std::make_unique<ui::Stalker>(sideColor));
}

rts::UnitId sc2::Factory::templar(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{3, 3}, UnitTypes::templar, sd, std::make_unique<ui::Templar>(sideColor));
}

rts::UnitId sc2::Factory::twilight(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{3, 3}, UnitTypes::twilight, sd, std::make_unique<ui::Twilight>(sideColor), 0,
      w.createProductionQueue(sd));
}

rts::UnitId sc2::Factory::zealot(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w[sd])};
  return w.units.emplace(
      rts::Vector{1, 1}, UnitTypes::zealot, sd, std::make_unique<ui::Zealot>(sideColor));
}

rts::ResourceFieldId sc2::Factory::geyser(rts::World& w) {
  return w.resourceFields.emplace(
      rts::Vector{3, 3}, Resources::gas, GeyserContent, rts::ResourceField::DestroyWhenEmpty::No,
      rts::ResourceField::RequiresBuilding::Yes, 3, std::make_unique<ui::Geyser>());
}

rts::ResourceFieldId sc2::Factory::mineralPatch(rts::World& w, rts::ResourceGroupId group) {
  return w.resourceFields.emplace(
      rts::Vector{2, 1}, Resources::mineral, MineralPatchContent,
      rts::ResourceField::DestroyWhenEmpty::Yes, rts::ResourceField::RequiresBuilding::No, 2,
      std::make_unique<ui::MineralPatch>(), group);
}

rts::BlockerId sc2::Factory::rock(rts::World& w, rts::Vector size) {
  return w.blockers.emplace(size, std::make_unique<ui::Rock>());
}
