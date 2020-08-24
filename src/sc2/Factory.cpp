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

rts::UnitId sc2::Factory::create(rts::World& w, rts::UnitTypeId t, rts::Point p, rts::SideId sd) {
  if (t == UnitTypes::nexus)
    return nexus(w, p, sd);
  if (t == UnitTypes::probe)
    return probe(w, p, sd);
  return {};
}

rts::UnitId sc2::Factory::nexus(rts::World& w, rts::Point p, rts::SideId sd) {
  auto sideColor{::ui::getColor(w.sides[sd])};
  return w.createUnit(
      p, rts::Vector{5, 5}, UnitTypes::nexus, sd, std::make_unique<ui::Nexus>(sideColor), 0,
      w.createProductionQueue(sd));
}

rts::UnitId sc2::Factory::probe(rts::World& w, rts::Point p, rts::SideId sd) {
  auto sideColor{::ui::getColor(w.sides[sd])};
  return w.createUnit(
      p, rts::Vector{1, 1}, UnitTypes::probe, sd, std::make_unique<ui::Probe>(sideColor),
      ProbeCargoCapacity);
}

rts::ResourceFieldId sc2::Factory::geyser(rts::World& w, rts::Point p) {
  return w.createResourceField(
      p, rts::Vector{3, 3}, Resources::gas, GeyserContent, std::make_unique<ui::Geyser>());
}

rts::ResourceFieldId sc2::Factory::mineralPatch(
    rts::World& w, rts::Point p, rts::ResourceGroupId group) {
  return w.createResourceField(
      p, rts::Vector{1, 1}, Resources::mineral, MineralPatchContent,
      std::make_unique<ui::MineralPatch>(), group);
}

rts::BlockerId sc2::Factory::rock(rts::World& w, rts::Point p) {
  return w.createBlocker(p, rts::Vector{1, 1}, std::make_unique<ui::Rock>());
}
