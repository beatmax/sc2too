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

  test::makeSides(world);
  Side& side1{world[test::side1Id]};
  Side& side2{world[test::side2Id]};
  REQUIRE(test::repr(side1.ui()) == "1");
  REQUIRE(test::repr(side2.ui()) == "2");
  REQUIRE(side1.resource(test::mineralResourceId).available() == 1000);
  REQUIRE(side2.resource(test::mineralResourceId).available() == 1000);

  REQUIRE(test::repr(cworld[test::moveAbilityId].ui) == "move");
  REQUIRE(test::repr(cworld[test::produceWorkerAbilityId].ui) == "p.w");
  REQUIRE(test::repr(cworld[test::produceThirdyAbilityId].ui) == "p.t");
  REQUIRE(test::repr(cworld[test::baseTypeId].ui) == "B");
  REQUIRE(test::repr(cworld[test::extractorTypeId].ui) == "E");
  REQUIRE(test::repr(cworld[test::workerTypeId].ui) == "W");
  REQUIRE(test::repr(cworld[test::thirdyTypeId].ui) == "T");

  world.loadMap(test::MapInitializer{}, std::istringstream{test::map});

  REQUIRE(cworld.map.size() == rts::Vector{40, 10});
  REQUIRE(cworld.map(9, 1).empty());
  REQUIRE(cworld.map(10, 1).contains(Cell::Blocker));
  REQUIRE(cworld.map(11, 1).contains(Cell::Blocker));
  REQUIRE(cworld.map(12, 1).contains(Cell::Blocker));
  REQUIRE(test::repr(cworld.blocker({12, 1})->ui) == "r");
  REQUIRE(cworld.map(13, 1).empty());

  const Rectangle baseArea{Point{37, 6}, rts::Vector{2, 3}};
  REQUIRE(cworld[baseArea.topLeft].contains(Cell::Unit));
  Unit* base{world.unit(baseArea.topLeft)};
  REQUIRE(base->area == baseArea);
  REQUIRE(test::repr(base->ui) == "b");
  for (Point p : baseArea.points()) {
    auto* u{cworld.unit(p)};
    REQUIRE(u != nullptr);
    REQUIRE(u == base);
  }

  const Rectangle geyserArea{Point{30, 0}, rts::Vector{2, 2}};
  REQUIRE(cworld[geyserArea.topLeft].contains(Cell::ResourceField));
  const ResourceField* geyser{cworld.resourceField(geyserArea.topLeft)};
  REQUIRE(geyser != nullptr);
  REQUIRE(geyser->area == geyserArea);
  REQUIRE(test::repr(geyser->ui) == "g");
  for (Point p : geyserArea.points()) {
    auto* rf{cworld.resourceField(p)};
    REQUIRE(rf != nullptr);
    REQUIRE(rf == geyser);
  }

  side1.resources().provision({{test::supplyResourceId, 100}});

  test::TestResources expectedResources;
  expectedResources.mineral.available = 1000;
  expectedResources.mineral.allocated = test::BaseMineralCost;
  expectedResources.supply.available = 100 + test::BaseSupplyProvision;
  REQUIRE(test::TestResources{side1} == expectedResources);

  SECTION("A unit is added to the world") {
    Point pos{20, 5};
    UnitId uid{world.add(test::Factory::worker(world, test::side1Id), pos)};
    REQUIRE(cworld[pos].contains(Cell::Unit));
    Unit& u{*world.unit(pos)};
    const Unit& cu{*cworld.unit(pos)};
    REQUIRE(cu.area.topLeft == pos);
    REQUIRE(test::repr(cu.ui) == "w");
    REQUIRE(test::Ui::count["w"] == 1);

    expectedResources.mineral.allocate(test::WorkerMineralCost);
    expectedResources.supply.allocate(test::WorkerSupplyCost);
    REQUIRE(test::TestResources{side1} == expectedResources);

    SECTION("The unit is destroyed") {
      pos = cu.area.topLeft;
      world.destroy(uid);
      REQUIRE(cworld[pos].empty());
      REQUIRE(test::Ui::count["w"] == 0);

      expectedResources.mineral.lose(test::WorkerMineralCost);
      expectedResources.supply.restore(test::WorkerSupplyCost);
      REQUIRE(test::TestResources{side1} == expectedResources);
    }

    SECTION("The 'move' ability is triggered") {
      const AbilityState& moveAbilityState{
          Unit::abilityState(UnitStableRef{u}, cworld, abilities::Kind::Move)};

      const Point targetPos{20, 3};
      u.trigger(test::MoveAbilityIndex, world, targetPos);

      ++world.time;
      test::stepUpdate(world, cu);

      REQUIRE(cu.area.topLeft == pos);
      REQUIRE(test::nextStepTime(cu) == GameTimeSecond + 1);

      SECTION("The path is found") {
        auto [path, isComplete] = findPath(world, pos, targetPos, {});
        REQUIRE(isComplete);
        REQUIRE(path == Path{{20, 4}, {20, 3}});
      }

      SECTION("The unit moves") {
        while (pos != targetPos) {
          world.time += GameTimeSecond;
          test::stepUpdate(world, cu);

          Point prevPos{pos};
          --pos.y;
          REQUIRE(cu.area.topLeft == pos);
          REQUIRE(cworld[prevPos].empty());
          REQUIRE(cworld[pos].contains(Cell::Unit));
          if (pos != targetPos) {
            REQUIRE(moveAbilityState.active());
            REQUIRE(moveAbilityState.nextStepTime() == world.time + GameTimeSecond);
          }
          else {
            // the last step deactivates the ability
            REQUIRE(!moveAbilityState.active());
            REQUIRE(moveAbilityState.nextStepTime() == GameTimeInf);
          }
          REQUIRE(test::nextStepTime(cu) == moveAbilityState.nextStepTime());
        }
      }

      SECTION("The unit is destroyed with pending actions on it") {
        world.time += GameTimeSecond;
        WorldActionList actions{Unit::step(UnitStableRef{cu}, cworld)};
        REQUIRE(!actions.empty());

        pos = cu.area.topLeft;
        world.destroy(uid);
        REQUIRE(cworld[pos].empty());
        REQUIRE(test::Ui::count["w"] == 0);

        world.update(actions);
      }
    }
  }

  SECTION("A multi-cell unit is added to the world") {
    const Rectangle area{Point{1, 1}, rts::Vector{2, 3}};

    REQUIRE(test::Ui::count["b"] == 1);

    UnitId uid{world.add(test::Factory::base(world, test::side1Id), area.topLeft)};
    const Unit& cu{*cworld.unit(area.topLeft)};
    REQUIRE(cu.area == area);
    REQUIRE(test::repr(cu.ui) == "b");
    REQUIRE(test::Ui::count["b"] == 2);

    const Rectangle outRect{area.topLeft - rts::Vector{1, 1}, area.size + rts::Vector{2, 2}};
    for (Point p : outRect.points()) {
      if (area.contains(p))
        REQUIRE(cworld.unit(p) == &cu);
      else
        REQUIRE(cworld[p].empty());
    }

    SECTION("The multi-cell unit is destroyed") {
      world.destroy(uid);
      REQUIRE(test::Ui::count["b"] == 1);
      for (Point p : area.points())
        REQUIRE(cworld[p].empty());
    }
  }

  SECTION("Move ability internals") {
    UnitId uid{world.add(test::Factory::worker(world, test::side1Id), Point{20, 5})};
    auto& unit{world[uid]};

    test::select(world, test::side1Id, {uid});

    SECTION("Path around a unit that appears after step calculation") {
      REQUIRE(
          test::runMove(world, unit, Point{27, 5}, 400) ==
          test::MoveStepList{
              {{20, 5}, 0}, {{21, 5}, 101}, {{22, 5}, 201}, {{23, 5}, 301}, {{23, 5}, 400}});
      ++world.time;
      WorldActionList actions{Unit::step(UnitStableRef{unit}, cworld)};
      REQUIRE(!actions.empty());
      world.add(test::Factory::base(world, test::side1Id), {24, 4});
      world.update(actions);
      REQUIRE(unit.area.topLeft == Point{23, 5});
      REQUIRE(
          test::continueMove(world, unit) ==
          test::MoveStepList{
              {{23, 5}, 401},
              {{23, 4}, 403},  // collision
              {{24, 3}, 544},
              {{25, 3}, 644},
              {{26, 4}, 785},
              {{27, 5}, 926}});
    }
  }

  SECTION("Energy increases periodically up to unit's maximum") {
    REQUIRE(base->energy == 50);
    base->energy = 198;
    test::step(world, 99);
    REQUIRE(base->energy == 198);
    world.step();
    REQUIRE(base->energy == 199);
    test::step(world, 99);
    REQUIRE(base->energy == 199);
    world.step();
    REQUIRE(base->energy == 200);
    test::step(world, 100);
    REQUIRE(base->energy == 200);
  }

  SECTION("Control groups and selection subgroups") {
    using ControlGroupCmd = command::ControlGroup;
    using SelectionSubgroupCmd = command::SelectionSubgroup;
    constexpr bool NonExclusive{false};
    constexpr bool Exclusive{true};
    auto groupUnits = [&](ControlGroupId g) { return side1.group(g).ids(world); };
    auto selectedUnits = [&]() { return side1.selection().ids(world); };
    auto subgroupType = [&]() { return side1.selection().subgroup(world).type; };

    UnitId u1{world.add(test::Factory::worker(world, test::side1Id), Point{21, 5})};
    UnitId u2{world.add(test::Factory::worker(world, test::side1Id), Point{22, 5})};
    UnitId u3{world.add(test::Factory::worker(world, test::side1Id), Point{23, 5})};

    REQUIRE(subgroupType() == UnitTypeId{});

    test::select(world, test::side1Id, {u1, u2, u3});
    REQUIRE(subgroupType() == test::workerTypeId);

    test::execCommand(world, test::side1Id, ControlGroupCmd{ControlGroupCmd::Set, NonExclusive, 1});
    REQUIRE(groupUnits(1) == UnitIdList{u1, u2, u3});
    REQUIRE(groupUnits(2) == UnitIdList{});

    test::select(world, test::side1Id, {u2, u3});
    test::execCommand(world, test::side1Id, ControlGroupCmd{ControlGroupCmd::Set, NonExclusive, 2});
    REQUIRE(groupUnits(1) == UnitIdList{u1, u2, u3});
    REQUIRE(groupUnits(2) == UnitIdList{u2, u3});

    test::execCommand(world, test::side1Id, ControlGroupCmd{ControlGroupCmd::Set, Exclusive, 3});
    REQUIRE(groupUnits(1) == UnitIdList{u1});
    REQUIRE(groupUnits(2) == UnitIdList{});
    REQUIRE(groupUnits(3) == UnitIdList{u2, u3});

    test::execCommand(world, test::side1Id, ControlGroupCmd{ControlGroupCmd::Add, NonExclusive, 1});
    REQUIRE(groupUnits(1) == UnitIdList{u1, u2, u3});
    REQUIRE(groupUnits(2) == UnitIdList{});
    REQUIRE(groupUnits(3) == UnitIdList{u2, u3});

    test::execCommand(world, test::side1Id, ControlGroupCmd{ControlGroupCmd::Add, Exclusive, 2});
    REQUIRE(groupUnits(1) == UnitIdList{u1});
    REQUIRE(groupUnits(2) == UnitIdList{u2, u3});
    REQUIRE(groupUnits(3) == UnitIdList{});

    test::execCommand(world, test::side1Id, ControlGroupCmd{ControlGroupCmd::Select, false, 1});
    REQUIRE(selectedUnits() == UnitIdList{u1});
    test::execCommand(world, test::side1Id, ControlGroupCmd{ControlGroupCmd::Select, false, 2});
    REQUIRE(selectedUnits() == UnitIdList{u2, u3});
    test::execCommand(world, test::side1Id, ControlGroupCmd{ControlGroupCmd::Select, false, 3});
    REQUIRE(selectedUnits() == UnitIdList{});
    REQUIRE(subgroupType() == UnitTypeId{});

    REQUIRE(groupUnits(1) == UnitIdList{u1});
    REQUIRE(groupUnits(2) == UnitIdList{u2, u3});
    REQUIRE(groupUnits(3) == UnitIdList{});

    test::select(world, test::side1Id, {u1}, command::Selection::Add);
    REQUIRE(selectedUnits() == UnitIdList{u1});
    REQUIRE(subgroupType() == test::workerTypeId);

    UnitId b1{world.units.id(*base)};
    UnitId t1{world.add(test::Factory::thirdy(world, test::side1Id), Point{24, 5})};
    UnitId t2{world.add(test::Factory::thirdy(world, test::side1Id), Point{25, 5})};

    test::select(world, test::side1Id, {u2, b1, t2, u1, t1});
    REQUIRE(selectedUnits() == UnitIdList{b1, u1, u2, t1, t2});
    REQUIRE(subgroupType() == test::baseTypeId);

    test::execCommand(world, test::side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Next});
    REQUIRE(subgroupType() == test::workerTypeId);
    test::execCommand(world, test::side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Next});
    REQUIRE(subgroupType() == test::thirdyTypeId);
    test::execCommand(world, test::side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Next});
    REQUIRE(subgroupType() == test::baseTypeId);
    test::execCommand(world, test::side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Previous});
    REQUIRE(subgroupType() == test::thirdyTypeId);
    test::execCommand(world, test::side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Previous});
    REQUIRE(subgroupType() == test::workerTypeId);

    world.destroy(u1);
    REQUIRE(selectedUnits() == UnitIdList{b1, u2, t1, t2});
    REQUIRE(subgroupType() == test::workerTypeId);

    world.destroy(u2);
    REQUIRE(selectedUnits() == UnitIdList{b1, t1, t2});
    REQUIRE(subgroupType() == UnitTypeId{});

    test::execCommand(world, test::side1Id, SelectionSubgroupCmd{SelectionSubgroupCmd::Next});
    REQUIRE(subgroupType() == test::baseTypeId);

    test::execCommand(world, test::side1Id, ControlGroupCmd{ControlGroupCmd::Select, false, 2});
    REQUIRE(selectedUnits() == UnitIdList{u3});
    REQUIRE(subgroupType() == test::workerTypeId);
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
      const auto side{test::side1Id};
      const auto unit{world.add(test::Factory::worker(world, side), Point{20, 5})};
      test::select(world, side, {unit});

      const GameTime frameTime{10};
      const GameTime finalGameTime{frameTime + 2 * GameTimeSecond};
      const auto finalElapsed{10 * 10ms + 2s};
      const auto pausedFrames{3};
      const auto totalFrames{210 + pausedFrames};

      const Point targetPos{20, 3};

      test::FakeController controller;
      int inputCalls{0}, outputCalls{0};

      auto processInput = [&](const World& w) -> std::optional<SideCommand> {
        ++inputCalls;
        if (inputCalls == 1)
          return SideCommand{side, rts::command::TriggerAbility{test::MoveAbilityIndex, targetPos}};
        else if (inputCalls == 100)
          controller.paused_ = true;
        else if (inputCalls == 100 + pausedFrames)
          controller.paused_ = false;
        else if (w.time >= finalGameTime - 1)
          controller.quit_ = true;
        return std::nullopt;
      };

      auto updateOutput = [&](const World&) { ++outputCalls; };

      engine.run(controller, processInput, updateOutput);

      REQUIRE(cworld[unit].area.topLeft == targetPos);
      REQUIRE(cworld.time == finalGameTime);
      REQUIRE(elapsed() == finalElapsed);
      REQUIRE(inputCalls == totalFrames);
      REQUIRE(outputCalls == totalFrames);
    }
  }
}
