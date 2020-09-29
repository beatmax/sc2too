#include "rts/Command.h"
#include "rts/Engine.h"
#include "rts/World.h"
#include "sc2/Abilities.h"
#include "sc2/Assets.h"
#include "sc2/Factory.h"
#include "sc2/Resources.h"
#include "sc2/UnitTypes.h"
#include "ui/IO.h"
#include "ui/SideUi.h"

#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>

namespace {
  std::vector<rts::SideId> makeSides(rts::World& w) {
    std::vector<rts::SideId> sides;
    for (int n = 0; n <= 1; ++n) {
      sides.push_back(
          w.createSide(sc2::Resources::initialResources(), std::make_unique<ui::SideUi>(n)));
    }
    return sides;
  }
}

int main() try {
  ui::IO io;

  sc2::Assets::init();
  auto worldPtr{rts::World::create(std::make_unique<sc2::Factory>())};
  rts::World& world{*worldPtr};

  const auto sides{makeSides(world)};
  const auto side{sides[0]};
  world.map.load(world, sc2::Assets::mapInitializer(), std::ifstream{"data/maps/ascii_jungle.txt"});
  ui::Player player{side, ui::Camera{{2, 6}, world.map.area()}};

  auto* nexus{world.closestUnit(player.camera.area().center(), side, sc2::UnitTypes::nexus)};
  assert(nexus);
  auto* closestMineral{world.closestResourceField(nexus->area.center(), sc2::Resources::mineral)};
  assert(closestMineral);
  nexus->trigger(sc2::Abilities::setRallyPoint, world, closestMineral->area.topLeft);

  for (int i = 0; i < 12; ++i) {
    auto p{world.emptyCellAround(nexus->area, closestMineral->area.center())};
    assert(p);
    auto probe{world.addForFree(sc2::Factory::probe(world, *p, side))};
    auto* mineral{world.closestResourceField(*p, sc2::Resources::mineral)};
    assert(mineral);
    world[probe].trigger(sc2::Abilities::gather, world, mineral->area.topLeft);
  }

  rts::Engine engine{world};
  engine.gameSpeed(2 * rts::GameSpeedNormal);
  engine.targetFps(100);

  auto processInput = [&](const rts::World& w, rts::SideCommandList& cmds) {
    io.input.process(engine, w, player, cmds);
  };
  auto updateOutput = [&](const rts::World& w) { io.output.update(engine, w, player); };

  engine.run(io, processInput, updateOutput);

} catch (const std::exception& e) {
  std::cerr << "ERROR: " << e.what() << std::endl;
}
