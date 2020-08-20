#include "catch2/catch.hpp"
#include "rts/Engine.h"
#include "rts/Path.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "rts/constants.h"
#include "tests-rts-assets.h"
#include "tests-rts-util.h"
#include "tests-util.h"
#include "util/Timer.h"

#include <memory>
#include <sstream>

using namespace Catch::Matchers;
using namespace rts;

TEST_CASE("Hello world!", "[rts]") {
  auto worldPtr{World::create(std::make_unique<test::Factory>())};
  World& world{*worldPtr};
  const World& cworld{world};

  REQUIRE(
      world.abilities.emplace(rts::Ability::TargetType::Any, std::make_unique<test::Ui>('m')) ==
      test::MoveAbilityId);
  REQUIRE(test::repr(world.abilities[test::MoveAbilityId].ui) == 'm');
  REQUIRE(
      world.abilities.emplace(rts::Ability::TargetType::None, std::make_unique<test::Ui>('0')) ==
      test::ProduceSimpletonAbilityId);
  REQUIRE(
      world.abilities.emplace(rts::Ability::TargetType::None, std::make_unique<test::Ui>('1')) ==
      test::ProduceThirdyAbilityId);

  REQUIRE(
      world.entityTypes.emplace(
          rts::ResourceQuantityList{}, GameTimeSecond, std::make_unique<test::Ui>('B')) ==
      test::BuildingTypeId);
  REQUIRE(
      world.entityTypes.emplace(
          rts::ResourceQuantityList{{&test::gas, test::SimpletonCost}}, test::SimpletonBuildTime,
          std::make_unique<test::Ui>('S')) == test::SimpletonTypeId);
  REQUIRE(
      world.entityTypes.emplace(
          rts::ResourceQuantityList{{&test::gas, test::ThirdyCost}}, test::ThirdyBuildTime,
          std::make_unique<test::Ui>('S')) == test::ThirdyTypeId);
  world.entityTypes[test::BuildingTypeId].addAbility(
      test::ProduceSimpletonAbilityIndex,
      abilities::Produce{test::ProduceSimpletonAbilityId, test::SimpletonTypeId});
  world.entityTypes[test::BuildingTypeId].addAbility(
      test::ProduceThirdyAbilityIndex,
      abilities::Produce{test::ProduceThirdyAbilityId, test::ThirdyTypeId});
  world.entityTypes[test::SimpletonTypeId].addAbility(
      test::MoveAbilityIndex, abilities::Move{test::MoveAbilityId, CellDistance / GameTimeSecond});

  auto sides{test::makeSides(world)};
  REQUIRE(sides.size() == 2);
  REQUIRE(sides[0] == test::Side1Id);
  REQUIRE(sides[1] == test::Side2Id);

  const Side& side1{cworld.sides[test::Side1Id]};
  const Side& side2{cworld.sides[test::Side2Id]};
  REQUIRE(side1.bag(&test::gas).quantity() == 1000);
  REQUIRE(test::repr(side1.ui()) == '1');
  REQUIRE(side2.bag(&test::gas).quantity() == 1000);
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
      const AbilityState& moveAbilityState{e.abilityState(cworld, abilities::Kind::Move)};

      const Point targetPos{20, 3};
      e.trigger(test::MoveAbilityId, world, targetPos);

      ++world.time;
      world.update(ce.step(cworld));

      REQUIRE(ce.area.topLeft == pos);
      REQUIRE(ce.nextStepTime == GameTimeSecond + 1);

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

  SECTION("Try some moves!") {
    EntityId eid{test::Factory::simpleton(world, Point{20, 5}, test::Side1Id)};
    auto& entity{world.entities[eid]};

    test::select(world, test::Side1Id, {eid});

    SECTION("Already there") {
      REQUIRE(
          test::runMove(world, entity, Point{20, 5}) ==
          test::MoveStepList{{{20, 5}, 0}, {{20, 5}, 2}});
    }

    SECTION("One cell straight") {
      REQUIRE(
          test::runMove(world, entity, Point{20, 6}) ==
          test::MoveStepList{{{20, 5}, 0}, {{20, 6}, 101}});
    }

    SECTION("One cell diagonal") {
      REQUIRE(
          test::runMove(world, entity, Point{21, 6}) ==
          test::MoveStepList{{{20, 5}, 0}, {{21, 6}, 142}});
    }

    SECTION("Straight line") {
      REQUIRE(
          test::runMove(world, entity, Point{20, 3}) ==
          test::MoveStepList{{{20, 5}, 0}, {{20, 4}, 101}, {{20, 3}, 201}});
    }

    SECTION("Straight and diagonal") {
      REQUIRE(
          test::runMove(world, entity, Point{21, 3}) ==
          test::MoveStepList{{{20, 5}, 0}, {{20, 4}, 101}, {{21, 3}, 242}});
    }

    SECTION("Path around the rocks") {
      REQUIRE(
          test::runMove(world, entity, Point{17, 7}) ==
          test::MoveStepList{{{20, 5}, 0},
                             {{19, 5}, 101},
                             {{18, 5}, 201},
                             {{17, 5}, 301},
                             {{16, 5}, 401},
                             {{15, 5}, 501},
                             {{14, 5}, 601},
                             {{13, 6}, 742},
                             {{13, 7}, 842},
                             {{14, 8}, 983},
                             {{15, 7}, 1124},
                             {{16, 7}, 1224},
                             {{17, 7}, 1324}});
    }

    SECTION("Path around an entity") {
      test::Factory::building(world, {24, 4}, test::Side1Id);
      REQUIRE(
          test::runMove(world, entity, Point{27, 5}) ==
          test::MoveStepList{{{20, 5}, 0},
                             {{21, 5}, 101},
                             {{22, 5}, 201},
                             {{23, 5}, 301},
                             {{23, 4}, 402},  // collision
                             {{24, 3}, 543},
                             {{25, 3}, 643},
                             {{26, 4}, 784},
                             {{27, 5}, 925}});
    }

    SECTION("Path around an entity that appears after step calculation") {
      REQUIRE(
          test::runMove(world, entity, Point{27, 5}, 400) ==
          test::MoveStepList{
              {{20, 5}, 0}, {{21, 5}, 101}, {{22, 5}, 201}, {{23, 5}, 301}, {{23, 5}, 400}});
      ++world.time;
      WorldActionList actions{entity.step(cworld)};
      REQUIRE(!actions.empty());
      test::Factory::building(world, {24, 4}, test::Side1Id);
      world.update(actions);
      REQUIRE(entity.area.topLeft == Point{23, 5});
      REQUIRE(
          test::continueMove(world, entity) ==
          test::MoveStepList{{{23, 5}, 401},
                             {{23, 4}, 403},  // collision
                             {{24, 3}, 544},
                             {{25, 3}, 644},
                             {{26, 4}, 785},
                             {{27, 5}, 926}});
    }

    SECTION("Path around adjacent entity") {
      test::Factory::simpleton(world, {21, 5}, test::Side1Id);
      REQUIRE(
          test::runMove(world, entity, Point{22, 5}) ==
          test::MoveStepList{{{20, 5}, 0}, {{21, 4}, 142}, {{22, 5}, 283}});
    }

    SECTION("Move next to target object") {
      REQUIRE(
          test::runMove(world, entity, Point{31, 0}) ==
          test::MoveStepList{{{20, 5}, 0},
                             {{21, 4}, 142},
                             {{22, 4}, 242},
                             {{23, 3}, 383},
                             {{24, 2}, 524},
                             {{25, 2}, 624},
                             {{26, 2}, 724},
                             {{27, 2}, 824},
                             {{28, 1}, 965},
                             {{29, 1}, 1065}});
    }

    SECTION("Move next to target blocker") {
      REQUIRE(
          test::runMove(world, entity, Point{16, 6}) ==
          test::MoveStepList{{{20, 5}, 0}, {{19, 5}, 101}, {{18, 5}, 201}, {{17, 5}, 301}});
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
          test::MoveStepList{{{20, 5}, 350}, {{21, 5}, 501}, {{21, 5}, 550}});
      for (Coordinate y : {4, 5, 6})
        test::Factory::simpleton(world, {22, y}, test::Side1Id);
      test::Factory::simpleton(world, {20, 5}, test::Side1Id);
      REQUIRE(
          test::continueMove(world, entity, 750) ==
          test::MoveStepList{{{21, 5}, 550}, {{21, 5}, 750}});
      world.destroy(world.entityId({22, 5}));
      REQUIRE(
          test::continueMove(world, entity) ==
          test::MoveStepList{{{21, 5}, 750}, {{22, 5}, 901}, {{23, 5}, 1001}});
    }
  }

  SECTION("Control groups and selection subgroups") {
    using ControlGroupCmd = command::ControlGroup;
    using SelectionSubgroupCmd = command::SelectionSubgroup;
    constexpr bool NonExclusive{false};
    constexpr bool Exclusive{true};
    auto groupEntities = [&](ControlGroupId g) { return side1.group(g).ids(world); };
    auto selectedEntities = [&]() { return side1.selection().ids(world); };
    auto subgroupType = [&]() { return side1.selection().subgroupType(world); };

    EntityId e1{test::Factory::simpleton(world, Point{21, 5}, test::Side1Id)};
    EntityId e2{test::Factory::simpleton(world, Point{22, 5}, test::Side1Id)};
    EntityId e3{test::Factory::simpleton(world, Point{23, 5}, test::Side1Id)};

    REQUIRE(subgroupType() == EntityTypeId{});

    test::select(world, test::Side1Id, {e1, e2, e3});
    REQUIRE(subgroupType() == test::SimpletonTypeId);

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
    REQUIRE(subgroupType() == EntityTypeId{});

    REQUIRE(groupEntities(1) == EntityIdList{e1});
    REQUIRE(groupEntities(2) == EntityIdList{e2, e3});
    REQUIRE(groupEntities(3) == EntityIdList{});

    test::select(world, test::Side1Id, {e1}, command::Selection::Add);
    REQUIRE(selectedEntities() == EntityIdList{e1});
    REQUIRE(subgroupType() == test::SimpletonTypeId);

    EntityId b1{world.entities.id(*building)};
    EntityId t1{test::Factory::thirdy(world, Point{24, 5}, test::Side1Id)};
    EntityId t2{test::Factory::thirdy(world, Point{25, 5}, test::Side1Id)};

    test::select(world, test::Side1Id, {e2, b1, t2, e1, t1});
    REQUIRE(selectedEntities() == EntityIdList{b1, e1, e2, t1, t2});
    REQUIRE(subgroupType() == test::BuildingTypeId);

    test::execCommand(world, test::Side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Next});
    REQUIRE(subgroupType() == test::SimpletonTypeId);
    test::execCommand(world, test::Side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Next});
    REQUIRE(subgroupType() == test::ThirdyTypeId);
    test::execCommand(world, test::Side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Next});
    REQUIRE(subgroupType() == test::BuildingTypeId);
    test::execCommand(world, test::Side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Previous});
    REQUIRE(subgroupType() == test::ThirdyTypeId);
    test::execCommand(world, test::Side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Previous});
    REQUIRE(subgroupType() == test::SimpletonTypeId);

    world.destroy(e1);
    REQUIRE(selectedEntities() == EntityIdList{b1, e2, t1, t2});
    REQUIRE(subgroupType() == test::SimpletonTypeId);

    world.destroy(e2);
    REQUIRE(selectedEntities() == EntityIdList{b1, t1, t2});
    REQUIRE(subgroupType() == EntityTypeId{});

    test::execCommand(world, test::Side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Next});
    REQUIRE(subgroupType() == test::BuildingTypeId);

    test::execCommand(world, test::Side1Id, ControlGroupCmd{ControlGroupCmd::Select, false, 2});
    REQUIRE(selectedEntities() == EntityIdList{e3});
    REQUIRE(subgroupType() == test::SimpletonTypeId);
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
      const auto side{test::Side1Id};
      const auto entity{test::Factory::simpleton(world, Point{20, 5}, side)};
      test::select(world, side, {entity});

      const GameTime frameTime{10};
      const GameTime finalGameTime{frameTime + 2 * GameTimeSecond};
      const auto finalElapsed{10 * 10ms + 2s};
      const auto pausedFrames{3};
      const auto totalFrames{210 + pausedFrames};

      const Point targetPos{20, 3};

      test::FakeController controller;
      int inputCalls{0}, outputCalls{0};

      auto processInput = [&](const World& w, rts::SideCommandList& cmds) {
        ++inputCalls;
        if (inputCalls == 1)
          addCommand(cmds, side, rts::command::TriggerAbility{test::MoveAbilityId, targetPos});
        else if (inputCalls == 100)
          controller.paused_ = true;
        else if (inputCalls == 100 + pausedFrames)
          controller.paused_ = false;
        else if (w.time >= finalGameTime - 1)
          controller.quit_ = true;
      };

      auto updateOutput = [&](const World&) { ++outputCalls; };

      engine.run(controller, processInput, updateOutput);

      REQUIRE(cworld[entity].area.topLeft == targetPos);
      REQUIRE(cworld.time == finalGameTime);
      REQUIRE(elapsed() == finalElapsed);
      REQUIRE(inputCalls == totalFrames);
      REQUIRE(outputCalls == totalFrames);
    }
  }

  SECTION("Production queue") {
    auto triggerSimpletonProduction = [&]() {
      test::execCommand(
          world, test::Side1Id, rts::command::TriggerAbility{test::ProduceSimpletonAbilityId, {}});
    };

    auto triggerThirdyProduction = [&]() {
      test::execCommand(
          world, test::Side1Id, rts::command::TriggerAbility{test::ProduceThirdyAbilityId, {}});
    };

    const Rectangle buildingArea{Point{1, 1}, rts::Vector{2, 3}};
    EntityId building{test::Factory::building(world, buildingArea.topLeft, test::Side1Id)};
    const Entity& cb{cworld[building]};
    test::select(world, test::Side1Id, {building});

    const ProductionQueue& queue{world[world[building].productionQueue]};
    auto queueWId{world.weakId(queue)};
    REQUIRE(queue.size() == 0);
    REQUIRE(test::Ui::count['s'] == 0);
    REQUIRE(test::Ui::count['t'] == 0);

    Quantity expectedGasLeft{1000};
    REQUIRE(side1.bag(&test::gas).quantity() == expectedGasLeft);

    SECTION("A unit is enqueued for production") {
      triggerSimpletonProduction();
      REQUIRE(cb.nextStepTime == world.time + 1);
      ++world.time;
      world.update(cb.step(cworld));
      expectedGasLeft -= test::SimpletonCost;

      REQUIRE(side1.bag(&test::gas).quantity() == expectedGasLeft);
      REQUIRE(queue.size() == 1);
      REQUIRE(test::Ui::count['s'] == 0);

      REQUIRE(cb.nextStepTime == world.time + test::SimpletonBuildTime);
      world.time += test::SimpletonBuildTime;
      world.update(cb.step(cworld));
      REQUIRE(queue.size() == 0);
      REQUIRE(test::Ui::count['s'] == 1);

      SECTION("Two units are enqueued for production") {
        triggerSimpletonProduction();
        triggerThirdyProduction();
        REQUIRE(cb.nextStepTime == world.time + 1);
        ++world.time;
        world.update(cb.step(cworld));
        expectedGasLeft -= test::SimpletonCost + test::ThirdyCost;

        REQUIRE(side1.bag(&test::gas).quantity() == expectedGasLeft);
        REQUIRE(queue.size() == 2);
        REQUIRE(test::Ui::count['s'] == 1);
        REQUIRE(test::Ui::count['t'] == 0);

        REQUIRE(cb.nextStepTime == world.time + test::SimpletonBuildTime);
        world.time += test::SimpletonBuildTime;
        world.update(cb.step(cworld));
        REQUIRE(queue.size() == 1);
        REQUIRE(test::Ui::count['s'] == 2);
        REQUIRE(test::Ui::count['t'] == 0);

        ++world.time;
        REQUIRE(cb.nextStepTime == world.time);
        world.update(cb.step(cworld));

        REQUIRE(cb.nextStepTime == world.time + test::ThirdyBuildTime);
        world.time += test::ThirdyBuildTime;
        world.update(cb.step(cworld));
        REQUIRE(queue.size() == 0);
        REQUIRE(test::Ui::count['s'] == 2);
        REQUIRE(test::Ui::count['t'] == 1);
      }

      SECTION("Units are enqueued during production") {
        triggerThirdyProduction();
        REQUIRE(cb.nextStepTime == world.time + 1);
        ++world.time;
        world.update(cb.step(cworld));
        expectedGasLeft -= test::ThirdyCost;

        REQUIRE(side1.bag(&test::gas).quantity() == expectedGasLeft);
        REQUIRE(queue.size() == 1);
        REQUIRE(test::Ui::count['s'] == 1);
        REQUIRE(test::Ui::count['t'] == 0);

        REQUIRE(cb.nextStepTime == world.time + test::ThirdyBuildTime);

        world.time += 5;
        triggerSimpletonProduction();
        expectedGasLeft -= test::SimpletonCost;

        REQUIRE(side1.bag(&test::gas).quantity() == expectedGasLeft);
        REQUIRE(queue.size() == 2);
        REQUIRE(test::Ui::count['s'] == 1);
        REQUIRE(test::Ui::count['t'] == 0);

        REQUIRE(cb.nextStepTime == world.time + (test::ThirdyBuildTime - 5));
        world.time += test::ThirdyBuildTime - 5;
        world.update(cb.step(cworld));
        REQUIRE(queue.size() == 1);
        REQUIRE(test::Ui::count['s'] == 1);
        REQUIRE(test::Ui::count['t'] == 1);

        REQUIRE(cb.nextStepTime == world.time + 1);
        ++world.time;
        world.update(cb.step(cworld));

        REQUIRE(cb.nextStepTime == world.time + test::SimpletonBuildTime);
        world.time += test::SimpletonBuildTime;
        world.update(cb.step(cworld));
        REQUIRE(queue.size() == 0);
        REQUIRE(test::Ui::count['s'] == 2);
        REQUIRE(test::Ui::count['t'] == 1);
      }
    }

    SECTION("Production is attempted with insufficient resources") {
      rts::ResourceBank dummyBank;
      world.sides[test::Side1Id].resources().tryTransferTo(dummyBank, {{&test::gas, 998}});
      expectedGasLeft = 2;
      REQUIRE(side1.bag(&test::gas).quantity() == expectedGasLeft);

      triggerSimpletonProduction();
      REQUIRE(side1.bag(&test::gas).quantity() == expectedGasLeft);
      REQUIRE(cb.nextStepTime == GameTimeInf);
      REQUIRE(side1.messages().size() == 1);
      REQUIRE_THAT(side1.messages()[0].text, Equals("Not enough gas!"));

      triggerThirdyProduction();
      expectedGasLeft = 0;
      REQUIRE(side1.bag(&test::gas).quantity() == expectedGasLeft);
    }

    world.destroy(building);
    REQUIRE(!world[queueWId]);
  }
}
