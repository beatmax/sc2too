#include "sc2/Factory.h"

#include "rts/abilities.h"
#include "sc2/Resources.h"
#include "sc2/ui.h"

rts::Entity sc2::Factory::nexus(rts::Point p, rts::SideCPtr sd) {
  rts::Entity e{p, rts::Vector{5, 5}, sd, std::make_unique<ui::Nexus>()};
  return e;
}

rts::Entity sc2::Factory::probe(rts::Point p, rts::SideCPtr sd) {
  rts::Entity e{p, rts::Vector{1, 1}, sd, std::make_unique<ui::Probe>()};
  e.addAbility(rts::abilities::move(rts::Speed{4}));
  return e;
}

rts::ResourceField sc2::Factory::geyser(rts::Point p) {
  return rts::ResourceField{
      p, rts::Vector{3, 3}, Resources::gas(), 2250, std::make_unique<ui::Geyser>()};
}

rts::ResourceField sc2::Factory::mineralPatch(rts::Point p) {
  return rts::ResourceField{
      p, rts::Vector{1, 1}, Resources::mineral(), 1800, std::make_unique<ui::MineralPatch>()};
}

rts::Blocker sc2::Factory::rock(rts::Point p) {
  return rts::Blocker{p, rts::Vector{1, 1}, std::make_unique<ui::Rock>()};
}
