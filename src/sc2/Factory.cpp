#include "sc2/Factory.h"

#include "rts/World.h"
#include "sc2/Abilities.h"
#include "sc2/Resources.h"
#include "sc2/UnitTypes.h"
#include "sc2/constants.h"
#include "sc2/ui.h"
#include "ui/SideUi.h"

void sc2::Factory::init(rts::World& w) {
  Resources::init(w);
  Abilities::init(w);
  UnitTypes::init(w);
}

rts::UnitId sc2::Factory::create(rts::World& w, rts::UnitTypeId t, rts::SideId sd) {
  if (t == UnitTypes::nexus)
    return nexus(w, sd);
  if (t == UnitTypes::probe)
    return probe(w, sd);
  if (t == UnitTypes::pylon)
    return pylon(w, sd);
  return {};
}

rts::UnitId sc2::Factory::nexus(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w.sides[sd])};
  return w.units.emplace(
      rts::Vector{5, 5}, UnitTypes::nexus, sd, std::make_unique<ui::Nexus>(sideColor), 0,
      w.createProductionQueue(sd));
}

rts::UnitId sc2::Factory::probe(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w.sides[sd])};
  return w.units.emplace(
      rts::Vector{1, 1}, UnitTypes::probe, sd, std::make_unique<ui::Probe>(sideColor),
      ProbeCargoCapacity);
}

rts::UnitId sc2::Factory::pylon(rts::World& w, rts::SideId sd) {
  auto sideColor{::ui::getColor(w.sides[sd])};
  return w.units.emplace(
      rts::Vector{2, 2}, UnitTypes::pylon, sd, std::make_unique<ui::Pylon>(sideColor));
}

rts::ResourceFieldId sc2::Factory::geyser(rts::World& w) {
  return w.resourceFields.emplace(
      rts::Vector{3, 3}, Resources::gas, GeyserContent, std::make_unique<ui::Geyser>());
}

rts::ResourceFieldId sc2::Factory::mineralPatch(rts::World& w, rts::ResourceGroupId group) {
  return w.resourceFields.emplace(
      rts::Vector{1, 1}, Resources::mineral, MineralPatchContent,
      std::make_unique<ui::MineralPatch>(), group);
}

rts::BlockerId sc2::Factory::rock(rts::World& w, rts::Vector size) {
  return w.blockers.emplace(size, std::make_unique<ui::Rock>());
}
