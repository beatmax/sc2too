#include "rts/Engine.h"
#include "rts/World.h"
#include "sc2/Assets.h"
#include "sc2/Probe.h"
#include "sc2/Resources.h"
#include "ui/IO.h"
#include "ui/SideUi.h"

#include <exception>
#include <fstream>
#include <iostream>

namespace {
  std::vector<rts::Side> makeSides() {
    std::vector<rts::Side> sides;
    const auto& resources = sc2::Resources::initialResources();
    for (int sideId = 0; sideId <= 1; ++sideId)
      sides.push_back(rts::Side{resources, std::make_unique<ui::SideUi>(sideId)});
    return sides;
  }
}

int main() try {
  ui::IO io;

  sc2::Assets::init();
  rts::World world{makeSides(), sc2::Assets::cellCreator(),
                   std::ifstream{"data/maps/ascii_jungle.txt"}};

  world.add(sc2::Probe::create(rts::Point{20, 10}, &world.sides[0]));

  ui::Player player{&world.sides[0], ui::Camera{{0, 0}, {world.map.maxX, world.map.maxY}}};

  rts::Engine engine{100};
  while (!io.quit()) {
    if (!io.paused())
      engine.advanceFrame();
    io.input.process(player);
    io.output.update(world, player);
  };

} catch (const std::exception& e) {
  std::cerr << "ERROR: " << e.what() << std::endl;
}
