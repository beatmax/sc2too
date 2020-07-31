#include "rts/Engine.h"
#include "rts/World.h"
#include "sc2/Abilities.h"
#include "sc2/Assets.h"
#include "sc2/EntityTypes.h"
#include "sc2/Factory.h"
#include "sc2/Resources.h"
#include "ui/IO.h"
#include "ui/SideUi.h"

#include <exception>
#include <fstream>
#include <iostream>

namespace {
  std::vector<rts::SideId> makeSides(rts::World& world) {
    std::vector<rts::SideId> sides;
    const auto& resources = sc2::Resources::initialResources();
    for (int n = 0; n <= 1; ++n)
      sides.push_back(world.createSide(resources, std::make_unique<ui::SideUi>(n)));
    return sides;
  }
}

int main() try {
  ui::IO io;

  sc2::Assets::init();
  auto worldPtr{rts::World::create()};
  rts::World& world{*worldPtr};

  sc2::Abilities::init(world);
  sc2::EntityTypes::init(world);

  const auto sides{makeSides(world)};
  world.map.load(world, sc2::Assets::mapInitializer(), std::ifstream{"data/maps/ascii_jungle.txt"});
  ui::Player player{sides[0], ui::Camera{{0, 0}, {world.map.maxX(), world.map.maxY()}}};

  forEachPoint(rts::Rectangle{{20, 11}, {2, 3}}, [&](rts::Point p) {
    player.selection.add(world, {sc2::Factory::probe(world, p, sides[0])});
  });

  rts::Engine engine{world};
  engine.gameSpeed(rts::GameSpeedNormal);
  engine.targetFps(100);

  auto processInput = [&](const rts::World& w) { return io.input.process(engine, w, player); };
  auto updateOutput = [&](const rts::World& w) { io.output.update(engine, w, player); };

  engine.run(io, processInput, updateOutput);

} catch (const std::exception& e) {
  std::cerr << "ERROR: " << e.what() << std::endl;
}
