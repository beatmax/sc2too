#include "rts/Command.h"
#include "rts/Engine.h"
#include "rts/World.h"
#include "sc2/Assets.h"
#include "sc2/Factory.h"
#include "sc2/Resources.h"
#include "ui/IO.h"
#include "ui/SideUi.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <memory>

namespace {
  std::vector<rts::SideId> makeSides(rts::World& w) {
    std::vector<rts::SideId> sides;
    const auto& resources = sc2::Resources::initialResources();
    for (int n = 0; n <= 1; ++n)
      sides.push_back(w.createSide(resources, std::make_unique<ui::SideUi>(n)));
    return sides;
  }
}

int main() try {
  ui::IO io;

  sc2::Assets::init();
  auto worldPtr{rts::World::create(std::make_unique<sc2::Factory>())};
  rts::World& world{*worldPtr};

  const auto sides{makeSides(world)};
  world.map.load(world, sc2::Assets::mapInitializer(), std::ifstream{"data/maps/ascii_jungle.txt"});
  ui::Player player{sides[0], ui::Camera{{0, 0}, {world.map.maxX(), world.map.maxY()}}};

  forEachPoint(rts::Rectangle{{20, 11}, {6, 6}}, [&](rts::Point p) {
    sc2::Factory::probe(world, p, sides[0]);
  });

  rts::Engine engine{world};
  engine.gameSpeed(4 * rts::GameSpeedNormal);
  engine.targetFps(100);

  auto processInput = [&](const rts::World& w, rts::SideCommandList& cmds) {
    io.input.process(engine, w, player, cmds);
  };
  auto updateOutput = [&](const rts::World& w) { io.output.update(engine, w, player); };

  engine.run(io, processInput, updateOutput);

} catch (const std::exception& e) {
  std::cerr << "ERROR: " << e.what() << std::endl;
}
