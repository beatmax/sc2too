#include "rts/Command.h"
#include "rts/Engine.h"
#include "rts/World.h"
#include "sc2/Abilities.h"
#include "sc2/Assets.h"
#include "sc2/Factory.h"
#include "sc2/Resources.h"
#include "sc2/UnitTypes.h"
#include "sc2/constants.h"
#include "ui/IO.h"
#include "ui/SideUi.h"
#include "util/ProgramOptions.h"

#include <cassert>
#include <exception>
#include <iostream>
#include <memory>

namespace {
  std::vector<rts::SideId> makeSides(rts::World& w) {
    std::vector<rts::SideId> sides;
    for (int n = 0; n <= 1; ++n) {
      sides.push_back(w.createSide(
          sc2::UnitTypes::nexus, sc2::Resources::initialResources(),
          std::make_unique<ui::SideUi>(n)));
    }
    return sides;
  }
}

int main(int argc, char** argv) try {
  const util::ProgramOptions opts{argc, argv};
  const auto mapfile{opts.arg("--map", "data/maps/ascii_jungle.txt")};
  const auto supervisor{!opts["--no-supervisor"]};

  if (supervisor)
    ui::IO::supervisor();

  ui::IO io;

  sc2::Assets::init();
  auto worldPtr{rts::World::create(std::make_unique<sc2::Factory>())};
  rts::World& world{*worldPtr};
  world.energyIncreaseTimer.setInterval(sc2::EnergyIncreaseInterval);

  const auto sides{makeSides(world)};
  const auto side{sides[0]};
  world.minimap.init(io.minimapSize());
  world.loadMap(sc2::Assets::mapInitializer(), mapfile);
  ui::Player player{side, ui::Camera{{2, 6}, world.map.area()}};

  auto* nexus{world.closestActiveUnit(player.camera.area().center(), side, sc2::UnitTypes::nexus)};
  assert(nexus);
  auto* closestMineral{world.closestResourceField(nexus->area.center(), sc2::Resources::mineral)};
  assert(closestMineral);
  nexus->trigger(sc2::Abilities::SetRallyPointIndex, world, closestMineral->area.topLeft);

  for (int i = 0; i < 12; ++i) {
    auto p{world.emptyCellAround(nexus->area, closestMineral->area.center())};
    assert(p);
    auto probe{world.addForFree(sc2::Factory::probe(world, side), *p)};
    auto* mineral{world.closestResourceField(*p, sc2::Resources::mineral)};
    assert(mineral);
    world[probe].trigger(sc2::Abilities::GatherIndex, world, world.id(*mineral));
  }

  rts::Engine engine{world, rts::GameSpeedFaster};
  engine.targetFps(60);

  auto processInput = [&](const rts::World& w) { return io.input.process(engine, w, player); };
  auto updateOutput = [&](const rts::World& w) { io.output.update(engine, w, player); };

  engine.run(io, processInput, updateOutput);

} catch (const std::exception& e) {
  std::cerr << "ERROR: " << e.what() << std::endl;
}
