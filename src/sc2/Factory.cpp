#include "sc2/Factory.h"

#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/Resources.h"
#include "sc2/ui.h"
#include "ui/SideUi.h"

rts::EntityId sc2::Factory::nexus(rts::World& w, rts::Point p, rts::SideId sd) {
  auto sideColor{::ui::getColor(w.sides[sd])};
  return w.createEntity(p, rts::Vector{5, 5}, sd, std::make_unique<ui::Nexus>(sideColor));
}

rts::EntityId sc2::Factory::probe(rts::World& w, rts::Point p, rts::SideId sd) {
  auto sideColor{::ui::getColor(w.sides[sd])};
  auto id{w.createEntity(p, rts::Vector{1, 1}, sd, std::make_unique<ui::Probe>(sideColor))};
  auto& e{w.entities[id]};
  e.addAbility(rts::abilities::move(rts::Speed{4}));
  return id;
}

rts::ResourceFieldId sc2::Factory::geyser(rts::World& w, rts::Point p) {
  return w.createResourceField(
      p, rts::Vector{3, 3}, Resources::gas(), 2250, std::make_unique<ui::Geyser>());
}

rts::ResourceFieldId sc2::Factory::mineralPatch(rts::World& w, rts::Point p) {
  return w.createResourceField(
      p, rts::Vector{1, 1}, Resources::mineral(), 1800, std::make_unique<ui::MineralPatch>());
}

rts::BlockerId sc2::Factory::rock(rts::World& w, rts::Point p) {
  return w.createBlocker(p, rts::Vector{1, 1}, std::make_unique<ui::Rock>());
}
