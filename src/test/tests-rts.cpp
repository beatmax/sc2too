#include "catch2/catch.hpp"
#include "rts/Engine.h"
#include "rts/Path.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "rts/dimensions.h"
#include "tests-rts-assets.h"
#include "tests-rts-util.h"
#include "tests-util.h"
#include "util/Timer.h"

#include <sstream>

using namespace Catch::Matchers;
using namespace rts;

TEST_CASE("Hello world!", "[rts]") {
  auto worldPtr{World::create()};
  World& world{*worldPtr};
  const World& cworld{world};

  REQUIRE(world.abilities.emplace(std::make_unique<test::Ui>('m')) == test::MoveAbilityId);
  REQUIRE(test::repr(world.abilities[test::MoveAbilityId].ui) == 'm');

  REQUIRE(world.entityTypes.emplace(std::make_unique<test::Ui>('S')) == test::SimpletonTypeId);
  REQUIRE(world.entityTypes.emplace(std::make_unique<test::Ui>('B')) == test::BuildingTypeId);
  world.entityTypes[test::SimpletonTypeId].abilities[0] =
      abilities::move(test::MoveAbilityId, CellDistance / GameTimeSecond);

  auto sides{test::makeSides(world)};
  REQUIRE(sides.size() == 2);
  REQUIRE(sides[0] == test::Side1Id);
  REQUIRE(sides[1] == test::Side2Id);

  const Side& side1{cworld.sides[test::Side1Id]};
  const Side& side2{cworld.sides[test::Side2Id]};
  REQUIRE(side1.quantity(&test::gas) == 0);
  REQUIRE(test::repr(side1.ui()) == '1');
  REQUIRE(side2.quantity(&test::gas) == 0);
  REQUIRE(test::repr(side2.ui()) == '2');

  world.map.load(world, test::MapInitializer{}, std::istringstream{test::map});

  REQUIRE(cworld.map.maxX() == 40);
  REQUIRE(cworld.map.maxY() == 10);
  REQUIRE(cworld.map(9, 1).empty());
  REQUIRE(cworld.map(10, 1).contains(Cell::Blocker));
  REQUIRE(cworld.map(11, 1).contains(Cell::Blocker));
  REQUIRE(cworld.map(12, 1).contains(Cell::Blocker));
  REQUIRE(test::repr(cworld.blocker({12, 1})->ui) == 'r');
  REQUIRE(cworld.map(13, 1).empty());

  const Rectangle buildingArea{Point{37, 6}, rts::Vector{2, 3}};
  REQUIRE(cworld[buildingArea.topLeft].contains(Cell::Entity));
  Entity* building{world.entity(buildingArea.topLeft)};
  REQUIRE(building->area == buildingArea);
  REQUIRE(test::repr(building->ui) == 'b');
  forEachPoint(buildingArea, [&](Point p) {
    auto* e{cworld.entity(p)};
    REQUIRE(e != nullptr);
    REQUIRE(e == building);
  });

  const Rectangle geyserArea{Point{30, 0}, rts::Vector{2, 2}};
  REQUIRE(cworld[geyserArea.topLeft].contains(Cell::ResourceField));
  const ResourceField* geyser{cworld.resourceField(geyserArea.topLeft)};
  REQUIRE(geyser != nullptr);
  REQUIRE(geyser->area == geyserArea);
  REQUIRE(test::repr(geyser->ui) == 'g');
  forEachPoint(geyserArea, [&](Point p) {
    auto* rf{cworld.resourceField(p)};
    REQUIRE(rf != nullptr);
    REQUIRE(rf == geyser);
  });

  SECTION("An entity is added to the world") {
    Point pos{20, 5};
    EntityId eid{test::Factory::simpleton(world, pos, test::Side1Id)};
    REQUIRE(cworld[pos].contains(Cell::Entity));
    Entity& e{*world.entity(pos)};
    const Entity& ce{*cworld.entity(pos)};
    REQUIRE(ce.area.topLeft == pos);
    REQUIRE(test::repr(ce.ui) == 's');
    REQUIRE(test::Ui::count['s'] == 1);

    SECTION("The entity is destroyed") {
      pos = ce.area.topLeft;
      world.destroy(eid);
      REQUIRE(cworld[pos].empty());
      REQUIRE(test::Ui::count['s'] == 0);
    }

    SECTION("The 'move' ability is triggered") {
      auto ai{world.entityTypes[e.type].abilityIndex(test::MoveAbilityId)};
      REQUIRE(ai != EntityAbilityIndex::None);
      AbilityState& moveAbilityState{e.abilityStates[ai]};

      const Point targetPos{20, 3};
      world.update(ce.trigger(test::MoveAbilityId, cworld, targetPos));
      REQUIRE(ce.area.topLeft == pos);
      REQUIRE(ce.nextStepTime == GameTimeSecond);

      SECTION("The path is found") {
        auto [path, isComplete] = findPath(world, pos, targetPos);
        REQUIRE(isComplete);
        REQUIRE(path == Path{{20, 4}, {20, 3}});
      }

      SECTION("The entity moves") {
        while (pos != targetPos) {
          world.time += GameTimeSecond;
          world.update(ce.step(cworld));

          Point prevPos{pos};
          --pos.y;
          REQUIRE(ce.area.topLeft == pos);
          REQUIRE(cworld[prevPos].empty());
          REQUIRE(cworld[pos].contains(Cell::Entity));
          if (pos != targetPos) {
            REQUIRE(moveAbilityState.active());
            REQUIRE(moveAbilityState.nextStepTime() == world.time + GameTimeSecond);
          }
          else {
            // the last step deactivates the ability
            REQUIRE(!moveAbilityState.active());
            REQUIRE(moveAbilityState.nextStepTime() == GameTimeInf);
          }
          REQUIRE(ce.nextStepTime == moveAbilityState.nextStepTime());
        }
      }

      SECTION("The entity is destroyed with pending actions on it") {
        world.time += GameTimeSecond;
        WorldActionList actions{ce.step(cworld)};
        REQUIRE(!actions.empty());

        pos = ce.area.topLeft;
        world.destroy(eid);
        REQUIRE(cworld[pos].empty());
        REQUIRE(test::Ui::count['s'] == 0);

        world.update(actions);
      }
    }
  }

  SECTION("A multi-cell entity is added to the world") {
    const Rectangle area{Point{1, 1}, rts::Vector{2, 3}};

    REQUIRE(test::Ui::count['b'] == 1);

    EntityId eid{test::Factory::building(world, area.topLeft, test::Side1Id)};
    const Entity& ce{*cworld.entity(area.topLeft)};
    REQUIRE(ce.area == area);
    REQUIRE(test::repr(ce.ui) == 'b');
    REQUIRE(test::Ui::count['b'] == 2);

    const Rectangle outRect{area.topLeft - rts::Vector{1, 1}, area.size + rts::Vector{2, 2}};
    forEachPoint(outRect, [&](Point p) {
      if (area.contains(p))
        REQUIRE(cworld.entity(p) == &ce);
      else
        REQUIRE(cworld[p].empty());
    });

    SECTION("The multi-cell entity is destroyed") {
      world.destroy(eid);
      REQUIRE(test::Ui::count['b'] == 1);
      forEachPoint(area, [&](Point p) { (cworld[p].empty()); });
    }
  }

  SECTION("Hello engine!") {
    using util::FakeClock;
    using namespace std::chrono_literals;

    const auto startTime{FakeClock::time};
    auto elapsed = [startTime]() { return FakeClock::time - startTime; };

    FakeClock::step = 0s;
    EngineBase<FakeClock> engine{world, GameSpeedNormal};

    REQUIRE(engine.gameSpeed() == 100);
    REQUIRE(engine.initialGameSpeed() == 100);
    REQUIRE(cworld.time == 0);

    SECTION("The engine generates 100 FPS while timely updating the world") {
      // 100 FPS = one frame every 10 milliseconds
      // at normal speed -> 1 game time unit per frame
      REQUIRE(engine.targetFps() == 100);
      REQUIRE(engine.fps() == 100);

      FakeClock::step = 100us;

      engine.advanceFrame();
      REQUIRE(cworld.time == 1);
      REQUIRE(elapsed() == 10ms);
      REQUIRE(engine.fps() == 100);

      engine.advanceFrame();
      REQUIRE(cworld.time == 2);
      REQUIRE(elapsed() == 20ms);
      REQUIRE(engine.fps() == 100);

      SECTION("Target FPS decreased to 20") {
        // 20 FPS = one frame every 50 milliseconds
        // at normal speed -> 5 game time units per frame
        engine.targetFps(20);
        REQUIRE(engine.targetFps() == 20);

        engine.advanceFrame();
        REQUIRE(cworld.time == 7);
        REQUIRE(elapsed() == 70ms);
        REQUIRE(engine.fps() == 20);

        engine.advanceFrame();
        REQUIRE(cworld.time == 12);
        REQUIRE(elapsed() == 120ms);
        REQUIRE(engine.fps() == 20);
      }

      SECTION("With increased game speed") {
        REQUIRE(cworld.time == 2);
        REQUIRE(engine.targetFps() == 100);

        FakeClock::step = 0s;
        engine.gameSpeed(1000);  // x10
        REQUIRE(engine.gameSpeed() == 1000);
        REQUIRE(engine.initialGameSpeed() == 100);
        FakeClock::step = 100us;

        engine.advanceFrame();
        REQUIRE(cworld.time == 12);
        REQUIRE(elapsed() == 30ms);
        REQUIRE(engine.fps() == 100);

        engine.advanceFrame();
        REQUIRE(cworld.time == 22);
        REQUIRE(elapsed() == 40ms);
        REQUIRE(engine.fps() == 100);

        SECTION("Target FPS increased to 200") {
          REQUIRE(cworld.time == 22);

          // 200 FPS = one frame every 5 milliseconds
          // at x10 speed -> 5 game time units per frame
          engine.targetFps(200);
          REQUIRE(engine.targetFps() == 200);

          engine.advanceFrame();
          REQUIRE(cworld.time == 27);
          REQUIRE(elapsed() == 45ms);
          REQUIRE(engine.fps() == 200);

          engine.advanceFrame();
          REQUIRE(cworld.time == 32);
          REQUIRE(elapsed() == 50ms);
          REQUIRE(engine.fps() == 200);

          SECTION("The engine saturates and FPS decrease") {
            FakeClock::step = 8ms;

            engine.advanceFrame();
            REQUIRE(cworld.time == 40);
            REQUIRE(elapsed() == 58ms);
            REQUIRE(engine.fps() == 125);

            engine.advanceFrame();
            REQUIRE(cworld.time == 48);
            REQUIRE(elapsed() == 66ms);
            REQUIRE(engine.fps() == 125);
          }
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
      auto eid{test::Factory::simpleton(world, Point{20, 5}, test::Side1Id)};
      auto& entity{world.entities[eid]};

      const GameTime frameTime{10};
      const GameTime finalGameTime{frameTime + 2 * GameTimeSecond};
      const auto finalElapsed{10 * 10ms + 2s};
      const auto pausedFrames{3};
      const auto totalFrames{210 + pausedFrames};

      const Point targetPos{20, 3};

      test::FakeController controller;
      int inputCalls{0}, outputCalls{0};

      auto processInput = [&](const World& w) -> WorldActionList {
        ++inputCalls;
        if (inputCalls == 1)
          return entity.trigger(test::MoveAbilityId, w, targetPos);
        else if (inputCalls == 100)
          controller.paused_ = true;
        else if (inputCalls == 100 + pausedFrames)
          controller.paused_ = false;
        else if (w.time >= finalGameTime)
          controller.quit_ = true;
        return {};
      };

      auto updateOutput = [&](const World&) { ++outputCalls; };

      engine.run(controller, processInput, updateOutput);

      REQUIRE(entity.area.topLeft == targetPos);
      REQUIRE(cworld.time == finalGameTime);
      REQUIRE(elapsed() == finalElapsed);
      REQUIRE(inputCalls == totalFrames);
      REQUIRE(outputCalls == totalFrames);
    }
  }

  SECTION("Try some moves!") {
    EntityId eid{test::Factory::simpleton(world, Point{20, 5}, test::Side1Id)};
    auto& entity{world.entities[eid]};

    test::select(world, test::Side1Id, {eid});

    SECTION("Already there") {
      REQUIRE(
          test::runMove(world, entity, Point{20, 5}) ==
          test::MoveStepList{{{20, 5}, 0}, {{20, 5}, 1}});
    }

    SECTION("One cell straight") {
      REQUIRE(
          test::runMove(world, entity, Point{20, 6}) ==
          test::MoveStepList{{{20, 5}, 0}, {{20, 6}, 100}});
    }

    SECTION("One cell diagonal") {
      REQUIRE(
          test::runMove(world, entity, Point{21, 6}) ==
          test::MoveStepList{{{20, 5}, 0}, {{21, 6}, 141}});
    }

    SECTION("Straight line") {
      REQUIRE(
          test::runMove(world, entity, Point{20, 3}) ==
          test::MoveStepList{{{20, 5}, 0}, {{20, 4}, 100}, {{20, 3}, 200}});
    }

    SECTION("Straight and diagonal") {
      REQUIRE(
          test::runMove(world, entity, Point{21, 3}) ==
          test::MoveStepList{{{20, 5}, 0}, {{20, 4}, 100}, {{21, 3}, 241}});
    }

    SECTION("Path around the rocks") {
      REQUIRE(
          test::runMove(world, entity, Point{17, 7}) ==
          test::MoveStepList{{{20, 5}, 0},
                             {{19, 5}, 100},
                             {{18, 5}, 200},
                             {{17, 5}, 300},
                             {{16, 5}, 400},
                             {{15, 5}, 500},
                             {{14, 5}, 600},
                             {{13, 6}, 741},
                             {{13, 7}, 841},
                             {{14, 8}, 982},
                             {{15, 7}, 1123},
                             {{16, 7}, 1223},
                             {{17, 7}, 1323}});
    }

    SECTION("Path around an entity") {
      test::Factory::building(world, {24, 4}, test::Side1Id);
      REQUIRE(
          test::runMove(world, entity, Point{27, 5}) ==
          test::MoveStepList{{{20, 5}, 0},
                             {{21, 5}, 100},
                             {{22, 5}, 200},
                             {{23, 5}, 300},
                             {{23, 4}, 401},  // collision
                             {{24, 3}, 542},
                             {{25, 3}, 642},
                             {{26, 4}, 783},
                             {{27, 5}, 924}});
    }

    SECTION("Path around an entity that appears after step calculation") {
      REQUIRE(
          test::runMove(world, entity, Point{27, 5}, 399) ==
          test::MoveStepList{
              {{20, 5}, 0}, {{21, 5}, 100}, {{22, 5}, 200}, {{23, 5}, 300}, {{23, 5}, 399}});
      ++world.time;
      WorldActionList actions{entity.step(cworld)};
      REQUIRE(!actions.empty());
      test::Factory::building(world, {24, 4}, test::Side1Id);
      world.update(actions);
      REQUIRE(entity.area.topLeft == Point{23, 5});
      REQUIRE(
          test::continueMove(world, entity) ==
          test::MoveStepList{{{23, 5}, 400},
                             {{23, 4}, 402},  // collision
                             {{24, 3}, 543},
                             {{25, 3}, 643},
                             {{26, 4}, 784},
                             {{27, 5}, 925}});
    }

    SECTION("Path around adjacent entity") {
      test::Factory::simpleton(world, {21, 5}, test::Side1Id);
      REQUIRE(
          test::runMove(world, entity, Point{22, 5}) ==
          test::MoveStepList{{{20, 5}, 0}, {{21, 4}, 141}, {{22, 5}, 282}});
    }

    SECTION("Move next to target object") {
      REQUIRE(
          test::runMove(world, entity, Point{31, 0}) ==
          test::MoveStepList{{{20, 5}, 0},
                             {{21, 4}, 141},
                             {{22, 4}, 241},
                             {{23, 3}, 382},
                             {{24, 2}, 523},
                             {{25, 2}, 623},
                             {{26, 2}, 723},
                             {{27, 2}, 823},
                             {{28, 1}, 964},
                             {{29, 1}, 1064}});
    }

    SECTION("Move next to target blocker") {
      REQUIRE(
          test::runMove(world, entity, Point{16, 6}) ==
          test::MoveStepList{{{20, 5}, 0}, {{19, 5}, 100}, {{18, 5}, 200}, {{17, 5}, 300}});
    }

    SECTION("Blocked and unblocked") {
      forEachPoint(Rectangle{{19, 4}, {3, 3}}, [&](Point p) {
        if (p != Point{20, 5})
          test::Factory::simpleton(world, p, test::Side1Id);
      });
      REQUIRE(
          test::runMove(world, entity, Point{23, 5}, 350) ==
          test::MoveStepList{{{20, 5}, 0}, {{20, 5}, 350}});
      world.destroy(world.entityId({21, 5}));
      REQUIRE(
          test::continueMove(world, entity, 550) ==
          test::MoveStepList{{{20, 5}, 350}, {{21, 5}, 500}, {{21, 5}, 550}});
      for (Coordinate y : {4, 5, 6})
        test::Factory::simpleton(world, {22, y}, test::Side1Id);
      test::Factory::simpleton(world, {20, 5}, test::Side1Id);
      REQUIRE(
          test::continueMove(world, entity, 750) ==
          test::MoveStepList{{{21, 5}, 550}, {{21, 5}, 750}});
      world.destroy(world.entityId({22, 5}));
      REQUIRE(
          test::continueMove(world, entity) ==
          test::MoveStepList{{{21, 5}, 750}, {{22, 5}, 900}, {{23, 5}, 1000}});
    }
  }

  SECTION("Control groups") {
    using ControlGroupCmd = command::ControlGroup;
    constexpr bool NonExclusive{false};
    constexpr bool Exclusive{true};
    auto groupEntities = [&](ControlGroupId g) { return side1.group(g).ids(world); };
    auto selectedEntities = [&]() { return side1.selection().ids(world); };

    EntityId e1{test::Factory::simpleton(world, Point{21, 5}, test::Side1Id)};
    EntityId e2{test::Factory::simpleton(world, Point{22, 5}, test::Side1Id)};
    EntityId e3{test::Factory::simpleton(world, Point{23, 5}, test::Side1Id)};

    test::select(world, test::Side1Id, {e1, e2, e3});
    test::execCommand(world, test::Side1Id, ControlGroupCmd{ControlGroupCmd::Set, NonExclusive, 1});
    REQUIRE(groupEntities(1) == EntityIdList{e1, e2, e3});
    REQUIRE(groupEntities(2) == EntityIdList{});

    test::select(world, test::Side1Id, {e2, e3});
    test::execCommand(world, test::Side1Id, ControlGroupCmd{ControlGroupCmd::Set, NonExclusive, 2});
    REQUIRE(groupEntities(1) == EntityIdList{e1, e2, e3});
    REQUIRE(groupEntities(2) == EntityIdList{e2, e3});

    test::execCommand(world, test::Side1Id, ControlGroupCmd{ControlGroupCmd::Set, Exclusive, 3});
    REQUIRE(groupEntities(1) == EntityIdList{e1});
    REQUIRE(groupEntities(2) == EntityIdList{});
    REQUIRE(groupEntities(3) == EntityIdList{e2, e3});

    test::execCommand(world, test::Side1Id, ControlGroupCmd{ControlGroupCmd::Add, NonExclusive, 1});
    REQUIRE(groupEntities(1) == EntityIdList{e1, e2, e3});
    REQUIRE(groupEntities(2) == EntityIdList{});
    REQUIRE(groupEntities(3) == EntityIdList{e2, e3});

    test::execCommand(world, test::Side1Id, ControlGroupCmd{ControlGroupCmd::Add, Exclusive, 2});
    REQUIRE(groupEntities(1) == EntityIdList{e1});
    REQUIRE(groupEntities(2) == EntityIdList{e2, e3});
    REQUIRE(groupEntities(3) == EntityIdList{});

    test::execCommand(world, test::Side1Id, ControlGroupCmd{ControlGroupCmd::Select, false, 1});
    REQUIRE(selectedEntities() == EntityIdList{e1});
    test::execCommand(world, test::Side1Id, ControlGroupCmd{ControlGroupCmd::Select, false, 2});
    REQUIRE(selectedEntities() == EntityIdList{e2, e3});
    test::execCommand(world, test::Side1Id, ControlGroupCmd{ControlGroupCmd::Select, false, 3});
    REQUIRE(selectedEntities() == EntityIdList{});

    REQUIRE(groupEntities(1) == EntityIdList{e1});
    REQUIRE(groupEntities(2) == EntityIdList{e2, e3});
    REQUIRE(groupEntities(3) == EntityIdList{});
  }
}
