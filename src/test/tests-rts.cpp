#include "catch2/catch.hpp"
#include "rts/Engine.h"
#include "rts/World.h"
#include "tests-rts-assets.h"
#include "tests-util.h"
#include "util/Timer.h"

#include <sstream>

using namespace Catch::Matchers;
using namespace rts;

TEST_CASE("Hello world!", "[rts]") {
  World world{test::makeSides(), test::CellCreator{}, std::istringstream{test::map}};
  const World& cworld{world};

  REQUIRE(cworld.map.maxX == 40);
  REQUIRE(cworld.map.maxY == 10);
  REQUIRE(isFree(cworld.map.at(9, 1)));
  REQUIRE(hasBlocker(cworld.map.at(10, 1)));
  REQUIRE(hasBlocker(cworld.map.at(11, 1)));
  REQUIRE(hasBlocker(cworld.map.at(12, 1)));
  REQUIRE(test::repr(getBlocker(cworld.map.at(12, 1)).ui()) == 'r');
  REQUIRE(isFree(cworld.map.at(13, 1)));

  const Rectangle buildingArea{Point{37, 6}, rts::Vector{2, 3}};
  REQUIRE(hasEntity(cworld.map.at(buildingArea.topLeft)));
  EntitySCPtr building{getEntity(cworld.map.at(buildingArea.topLeft))};
  REQUIRE(building->area == buildingArea);
  REQUIRE(test::repr(building->ui()) == 'b');
  forEachPoint(buildingArea, [&](Point p) {
    REQUIRE(hasEntity(cworld.map.at(p)));
    REQUIRE(getEntity(cworld.map.at(p)) == building);
  });

  const Rectangle geyserArea{Point{30, 0}, rts::Vector{2, 2}};
  REQUIRE(hasResourceField(cworld.map.at(geyserArea.topLeft)));
  ResourceFieldSCPtr geyser{getResourceField(cworld.map.at(geyserArea.topLeft))};
  REQUIRE(geyser->area == geyserArea);
  REQUIRE(test::repr(geyser->ui()) == 'g');
  forEachPoint(geyserArea, [&](Point p) {
    REQUIRE(hasResourceField(cworld.map.at(p)));
    REQUIRE(getResourceField(cworld.map.at(p)) == geyser);
  });

  REQUIRE(cworld.sides.size() == 2);
  REQUIRE(cworld.sides[0].quantity(&test::gas) == 0);
  REQUIRE(test::repr(cworld.sides[0].ui()) == '0');
  REQUIRE(cworld.sides[1].quantity(&test::gas) == 0);
  REQUIRE(test::repr(cworld.sides[1].ui()) == '1');

  const SideCPtr side0{&cworld.sides[0]};

  SECTION("An entity is added to the world") {
    Point pos{20, 5};
    world.add(test::Simpleton::create(pos, side0));
    REQUIRE(hasEntity(cworld.map.at(pos)));
    EntitySCPtr ce{getEntity(cworld.map.at(pos))};
    REQUIRE(ce->area.topLeft == pos);
    REQUIRE(test::repr(ce->ui()) == 's');

    SECTION("The entity is destroyed") {
      world.destroy(ce);
      REQUIRE(isFree(cworld.map.at(ce->area.topLeft)));
      REQUIRE(ce.use_count() == 1);
    }

    SECTION("The 'move' ability is triggered") {
      EntitySPtr e{getEntity(world.map.at(pos))};
      REQUIRE(e->abilities.size() > 0);
      Ability& moveAbility = e->abilities.front();
      REQUIRE(moveAbility.name() == "move");

      world.time = 1;
      const Point targetPos{20, 3};
      world.update(Entity::trigger(moveAbility, cworld, e, targetPos));
      REQUIRE(ce->area.topLeft == pos);

      SECTION("The entity moves") {
        while (pos != targetPos) {
          world.time += GameTimeSecond;
          world.update(Entity::step(cworld, e));

          Point prevPos{pos};
          --pos.y;
          REQUIRE(ce->area.topLeft == pos);
          REQUIRE(isFree(cworld.map.at(prevPos)));
          REQUIRE(hasEntity(cworld.map.at(pos)));
          REQUIRE(moveAbility.active());
        }

        // the next step deactivates the ability
        world.time += GameTimeSecond;
        world.update(Entity::step(cworld, e));
        REQUIRE(ce->area.topLeft == targetPos);
        REQUIRE(!moveAbility.active());
        REQUIRE(moveAbility.nextStepTime() == 0);
      }

      SECTION("The entity is destroyed with pending actions on it") {
        world.time += GameTimeSecond;
        WorldActionList actions{Entity::step(cworld, e)};
        REQUIRE(!actions.empty());
        e.reset();
        REQUIRE(ce.use_count() == 3);
        world.destroy(ce);
        REQUIRE(ce.use_count() == 1);
        ce.reset();
        world.update(actions);
      }
    }
  }

  SECTION("A multi-cell entity is added to the world") {
    const Rectangle area{Point{1, 1}, rts::Vector{2, 3}};

    world.add(test::Building::create(area.topLeft, side0));
    EntitySCPtr ce{getEntity(cworld.map.at(area.topLeft))};
    REQUIRE(ce->area == area);

    const Rectangle outRect{area.topLeft - rts::Vector{1, 1}, area.size + rts::Vector{2, 2}};
    forEachPoint(outRect, [&](Point p) {
      if (area.contains(p)) {
        REQUIRE(hasEntity(cworld.map.at(p)));
        REQUIRE(getEntity(cworld.map.at(p)) == ce);
      }
      else {
        REQUIRE(isFree(cworld.map.at(p)));
      }
    });

    SECTION("The multi-cell entity is destroyed") {
      world.destroy(ce);
      forEachPoint(area, [&](Point p) { REQUIRE(isFree(cworld.map.at(p))); });
      REQUIRE(ce.use_count() == 1);
      ce.reset();
    }
  }

  SECTION("Hello engine!") {
    using util::FakeClock;
    using namespace std::chrono_literals;

    const auto startTime{FakeClock::time};
    auto elapsed = [startTime]() { return FakeClock::time - startTime; };

    FakeClock::step = 0s;
    EngineBase<FakeClock> engine{world};

    REQUIRE(engine.gameSpeed() == 1000);
    REQUIRE(engine.initialGameSpeed() == 1000);
    REQUIRE(cworld.time == 0);

    SECTION("The engine generates 100 FPS while timely updating the world") {
      // 100 FPS = one frame every 10 milliseconds
      // at normal speed -> 10 game time units per frame
      REQUIRE(engine.targetFps() == 100);
      REQUIRE(engine.fps() == 100);

      FakeClock::step = 100us;

      engine.advanceFrame();
      REQUIRE(cworld.time == 10);
      REQUIRE(elapsed() == 10ms);
      REQUIRE(engine.fps() == 100);

      engine.advanceFrame();
      REQUIRE(cworld.time == 20);
      REQUIRE(elapsed() == 20ms);
      REQUIRE(engine.fps() == 100);

      SECTION("With increased game speed") {
        FakeClock::step = 0s;
        engine.gameSpeed(1200);  // 20% faster
        REQUIRE(engine.gameSpeed() == 1200);
        REQUIRE(engine.initialGameSpeed() == 1000);
        FakeClock::step = 100us;

        engine.advanceFrame();
        REQUIRE(cworld.time == 32);
        REQUIRE(elapsed() == 30ms);
        REQUIRE(engine.fps() == 100);

        engine.advanceFrame();
        REQUIRE(cworld.time == 44);
        REQUIRE(elapsed() == 40ms);
        REQUIRE(engine.fps() == 100);
      }

      SECTION("Target FPS increased to 200") {
        REQUIRE(cworld.time == 20);

        // 200 FPS = one frame every 5 milliseconds
        // at normal speed -> 5 game time units per frame
        engine.targetFps(200);
        REQUIRE(engine.targetFps() == 200);

        engine.advanceFrame();
        REQUIRE(cworld.time == 25);
        REQUIRE(elapsed() == 25ms);
        REQUIRE(engine.fps() == 200);

        engine.advanceFrame();
        REQUIRE(cworld.time == 30);
        REQUIRE(elapsed() == 30ms);
        REQUIRE(engine.fps() == 200);

        SECTION("The engine saturates and FPS decrease") {
          FakeClock::step = 8ms;

          engine.advanceFrame();
          REQUIRE(cworld.time == 38);
          REQUIRE(elapsed() == 38ms);
          REQUIRE(engine.fps() == 125);

          engine.advanceFrame();
          REQUIRE(cworld.time == 46);
          REQUIRE(elapsed() == 46ms);
          REQUIRE(engine.fps() == 125);
        }
      }
    }

    SECTION("A very long and slow game") {
      FakeClock::step = 1h;  // one frame per hour
      engine.gameSpeed(1);   // 3600 world updates per frame

      auto expectedGameTime{cworld.time};
      auto expectedElapsed{elapsed()};
      for (int i = 0; i < 10; ++i) {
        engine.advanceFrame();
        expectedGameTime += 3600;
        expectedElapsed += 1h;
        REQUIRE(cworld.time == expectedGameTime);
        REQUIRE(elapsed() == expectedElapsed);
        REQUIRE(engine.fps() == 0);
      }
    }

    SECTION("The engine runs and updates the world") {
      auto entity{test::Simpleton::create(Point{20, 5}, side0)};
      world.add(entity);

      const GameTime frameTime{10};
      const GameTime finalGameTime{frameTime + 2 * GameTimeSecond};
      const auto finalElapsed{10ms + 2s};
      const auto pausedFrames{3};
      const auto totalFrames{201 + pausedFrames};

      Ability& moveAbility = entity->abilities.front();
      REQUIRE(moveAbility.name() == "move");
      const Point targetPos{20, 3};

      test::FakeController controller;
      int inputCalls{0}, outputCalls{0};

      auto processInput = [&](const rts::World& w) -> WorldActionList {
        ++inputCalls;
        if (inputCalls == 1)
          return Entity::trigger(moveAbility, w, entity, targetPos);
        else if (inputCalls == 100)
          controller.paused_ = true;
        else if (inputCalls == 100 + pausedFrames)
          controller.paused_ = false;
        else if (w.time >= finalGameTime)
          controller.quit_ = true;
        return {};
      };

      auto updateOutput = [&](const rts::World&) { ++outputCalls; };

      engine.run(controller, processInput, updateOutput);

      REQUIRE(entity->area.topLeft == targetPos);
      REQUIRE(cworld.time == finalGameTime);
      REQUIRE(elapsed() == finalElapsed);
      REQUIRE(inputCalls == totalFrames);
      REQUIRE(outputCalls == totalFrames);
    }
  }
}
