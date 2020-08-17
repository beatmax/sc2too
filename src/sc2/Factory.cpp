#include "sc2/Factory.h"

#include "rts/World.h"
#include "sc2/EntityTypes.h"
#include "sc2/Resources.h"
#include "sc2/constants.h"
#include "sc2/ui.h"
#include "ui/SideUi.h"

rts::EntityId sc2::Factory::create(
    rts::World& w, rts::EntityTypeId t, rts::Point p, rts::SideId sd) {
  if (t == EntityTypes::nexus)
    return nexus(w, p, sd);
  if (t == EntityTypes::probe)
    return probe(w, p, sd);
  return {};
}

rts::EntityId sc2::Factory::nexus(rts::World& w, rts::Point p, rts::SideId sd) {
  auto sideColor{::ui::getColor(w.sides[sd])};
  return w.createEntity(
      p, rts::Vector{5, 5}, EntityTypes::nexus, sd, std::make_unique<ui::Nexus>(sideColor), 0,
      w.createProductionQueue(sd));
}

rts::EntityId sc2::Factory::probe(rts::World& w, rts::Point p, rts::SideId sd) {
  auto sideColor{::ui::getColor(w.sides[sd])};
  return w.createEntity(
      p, rts::Vector{1, 1}, EntityTypes::probe, sd, std::make_unique<ui::Probe>(sideColor),
      ProbeCargoCapacity);
}

rts::ResourceFieldId sc2::Factory::geyser(rts::World& w, rts::Point p) {
  return w.createResourceField(
      p, rts::Vector{3, 3}, Resources::gas(), GeyserContent, std::make_unique<ui::Geyser>());
}

rts::ResourceFieldId sc2::Factory::mineralPatch(
    rts::World& w, rts::Point p, rts::ResourceGroupId group) {
  return w.createResourceField(
      p, rts::Vector{1, 1}, Resources::mineral(), MineralPatchContent,
      std::make_unique<ui::MineralPatch>(), group);
}

rts::BlockerId sc2::Factory::rock(rts::World& w, rts::Point p) {
  return w.createBlocker(p, rts::Vector{1, 1}, std::make_unique<ui::Rock>());
}
