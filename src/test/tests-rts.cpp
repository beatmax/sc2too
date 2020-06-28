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
  auto worldPtr{
      World::create(test::makeSides(), test::MapInitializer{}, std::istringstream{test::map})};
  World& world{*worldPtr};
  const World& cworld{world};

  REQUIRE(cworld.map.maxX == 40);
  REQUIRE(cworld.map.maxY == 10);
  REQUIRE(isFree(cworld.map.at(9, 1)));
  REQUIRE(hasBlocker(cworld.map.at(10, 1)));
  REQUIRE(hasBlocker(cworld.map.at(11, 1)));
  REQUIRE(hasBlocker(cworld.map.at(12, 1)));
  REQUIRE(test::repr(cworld.blockerAt({12, 1}).ui) == 'r');
  REQUIRE(isFree(cworld.map.at(13, 1)));

  const Rectangle buildingArea{Point{37, 6}, rts::Vector{2, 3}};
  REQUIRE(hasEntity(cworld.map.at(buildingArea.topLeft)));
  Entity& building{world.entityAt(buildingArea.topLeft)};
  REQUIRE(building.area == buildingArea);
  REQUIRE(test::repr(building.ui) == 'b');
  forEachPoint(buildingArea, [&](Point p) {
    REQUIRE(hasEntity(cworld.map.at(p)));
    REQUIRE(&world.entityAt(p) == &building);
  });

  const Rectangle geyserArea{Point{30, 0}, rts::Vector{2, 2}};
  REQUIRE(hasResourceField(cworld.map.at(geyserArea.topLeft)));
  const ResourceField& geyser{cworld.resourceFieldAt(geyserArea.topLeft)};
  REQUIRE(geyser.area == geyserArea);
  REQUIRE(test::repr(geyser.ui) == 'g');
  forEachPoint(geyserArea, [&](Point p) {
    REQUIRE(hasResourceField(cworld.map.at(p)));
    REQUIRE(&cworld.resourceFieldAt(p) == &geyser);
  });

  REQUIRE(cworld.sides.size() == 2);
  REQUIRE(cworld.sides[0].quantity(&test::gas) == 0);
  REQUIRE(test::repr(cworld.sides[0].ui()) == '0');
  REQUIRE(cworld.sides[1].quantity(&test::gas) == 0);
  REQUIRE(test::repr(cworld.sides[1].ui()) == '1');

  const SideCPtr side0{&cworld.sides[0]};

  SECTION("An entity is added to the world") {
    Point pos{20, 5};
    EntityId eid{world.add(test::Factory::simpleton(pos, side0))};
    EntityWId ewid{cworld.entities, eid};
    REQUIRE(hasEntity(cworld.map.at(pos)));
    Entity& e{world.entityAt(pos)};
    const Entity& ce{cworld.entityAt(pos)};
    REQUIRE(ce.area.topLeft == pos);
    REQUIRE(test::repr(ce.ui) == 's');
    REQUIRE(test::Ui::count['s'] == 1);

    SECTION("The entity is destroyed") {
      pos = ce.area.topLeft;
      world.destroy(ewid);
      REQUIRE(isFree(cworld.map.at(pos)));
      REQUIRE(test::Ui::count['s'] == 0);
    }

    SECTION("The 'move' ability is triggered") {
      REQUIRE(e.abilities.size() > 0);
      Ability& moveAbility = e.abilities.front();
      REQUIRE(moveAbility.name() == "move");

      const Point targetPos{20, 3};
      world.update(ce.trigger(moveAbility, cworld, targetPos));
      REQUIRE(ce.area.topLeft == pos);
      REQUIRE(ce.nextStepTime == GameTimeSecond);

      SECTION("The entity moves") {
        while (pos != targetPos) {
          world.time += GameTimeSecond;
          world.update(ce.step(cworld));

          Point prevPos{pos};
          --pos.y;
          REQUIRE(ce.area.topLeft == pos);
          REQUIRE(isFree(cworld.map.at(prevPos)));
          REQUIRE(hasEntity(cworld.map.at(pos)));
          REQUIRE(moveAbility.active());
          REQUIRE(moveAbility.nextStepTime() == world.time + GameTimeSecond);
          REQUIRE(ce.nextStepTime == moveAbility.nextStepTime());
        }

        // the next step deactivates the ability
        world.time += GameTimeSecond;
        world.update(ce.step(cworld));
        REQUIRE(ce.area.topLeft == targetPos);
        REQUIRE(!moveAbility.active());
        REQUIRE(moveAbility.nextStepTime() == rts::GameTimeInf);
        REQUIRE(ce.nextStepTime == rts::GameTimeInf);
      }

      SECTION("The entity is destroyed with pending actions on it") {
        world.time += GameTimeSecond;
        WorldActionList actions{ce.step(cworld)};
        REQUIRE(!actions.empty());

        pos = ce.area.topLeft;
        world.destroy(ewid);
        REQUIRE(isFree(cworld.map.at(pos)));
        REQUIRE(test::Ui::count['s'] == 0);

        world.update(actions);
      }
    }
  }

  SECTION("A multi-cell entity is added to the world") {
    const Rectangle area{Point{1, 1}, rts::Vector{2, 3}};

    REQUIRE(test::Ui::count['b'] == 1);

    EntityId eid{world.add(test::Factory::building(area.topLeft, side0))};
    EntityWId ewid{cworld.entities, eid};
    const Entity& ce{cworld.entityAt(area.topLeft)};
    REQUIRE(ce.area == area);
    REQUIRE(test::repr(ce.ui) == 'b');
    REQUIRE(test::Ui::count['b'] == 2);

    const Rectangle outRect{area.topLeft - rts::Vector{1, 1}, area.size + rts::Vector{2, 2}};
    forEachPoint(outRect, [&](Point p) {
      if (area.contains(p)) {
        REQUIRE(hasEntity(cworld.map.at(p)));
        REQUIRE(&cworld.entityAt(p) == &ce);
      }
      else {
        REQUIRE(isFree(cworld.map.at(p)));
      }
    });

    SECTION("The multi-cell entity is destroyed") {
      world.destroy(ewid);
      REQUIRE(test::Ui::count['b'] == 1);
      forEachPoint(area, [&](Point p) { REQUIRE(isFree(cworld.map.at(p))); });
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
      auto eid{world.add(test::Factory::simpleton(Point{20, 5}, side0))};
      auto& entity{world.entities[eid]};

      const GameTime frameTime{10};
      const GameTime finalGameTime{frameTime + 2 * GameTimeSecond};
      const auto finalElapsed{10ms + 2s};
      const auto pausedFrames{3};
      const auto totalFrames{201 + pausedFrames};

      Ability& moveAbility = entity.abilities.front();
      REQUIRE(moveAbility.name() == "move");
      const Point targetPos{20, 3};

      test::FakeController controller;
      int inputCalls{0}, outputCalls{0};

      auto processInput = [&](const rts::World& w) -> WorldActionList {
        ++inputCalls;
        if (inputCalls == 1)
          return entity.trigger(moveAbility, w, targetPos);
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

      REQUIRE(entity.area.topLeft == targetPos);
      REQUIRE(cworld.time == finalGameTime);
      REQUIRE(elapsed() == finalElapsed);
      REQUIRE(inputCalls == totalFrames);
      REQUIRE(outputCalls == totalFrames);
    }
  }
}
