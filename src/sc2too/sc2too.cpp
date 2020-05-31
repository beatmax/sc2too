#include "rts/World.h"
#include "sc2/Assets.h"
#include "sc2/Probe.h"
#include "ui/Input.h"
#include "ui/Output.h"

#include <exception>
#include <fstream>
#include <iostream>

int main() try {
  ui::Output output;

  sc2::Assets::init();
  rts::World world{
      rts::Map{sc2::Assets::cellCreator(), std::ifstream{"data/maps/ascii_jungle.txt"}}};

  world.add(sc2::Probe::create(rts::Position{20, 10}));

  ui::Player player{ui::Camera{{0, 0}, {world.map.maxX, world.map.maxY}}};
  ui::Input input{player};

  do {
    output.update(world, player);
  } while (input.process());

} catch (const std::exception& e) {
  std::cerr << "ERROR: " << e.what() << std::endl;
}
