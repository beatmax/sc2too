#include "rts/World.h"
#include "sc2/Assets.h"
#include "sc2/Probe.h"
#include "sc2/Resources.h"
#include "ui/IO.h"

#include <exception>
#include <fstream>
#include <iostream>

int main() try {
  ui::IO io;

  sc2::Assets::init();
  rts::World world{
      rts::Map{sc2::Assets::cellCreator(), std::ifstream{"data/maps/ascii_jungle.txt"}}, 1,
      sc2::Resources::initialResources()};

  world.add(sc2::Probe::create(rts::Point{20, 10}));

  ui::Player player{world.sides[0], ui::Camera{{0, 0}, {world.map.maxX, world.map.maxY}}};

  do {
    io.output.update(world, player);
  } while (io.input.process(player));

} catch (const std::exception& e) {
  std::cerr << "ERROR: " << e.what() << std::endl;
}
