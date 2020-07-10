#include "tests-rts-util.h"

#include "catch2/catch.hpp"
#include "rts/Entity.h"
#include "rts/World.h"

using namespace rts;

test::MoveStepList test::runMove(World& world, Entity& entity, Point target, GameTime timeout) {
  REQUIRE(entity.abilities.size() > 0);
  Ability& moveAbility = entity.abilities.front();
  REQUIRE(moveAbility.name() == "move");

  world.update(entity.trigger(moveAbility, world, target));
  return continueMove(world, entity, timeout);
}

test::MoveStepList test::continueMove(World& world, Entity& entity, GameTime timeout) {
  Ability& moveAbility = entity.abilities.front();

  MoveStepList result;
  result.emplace_back(entity.area.topLeft, world.time);

  while (moveAbility.active() && world.time < timeout) {
    ++world.time;
    world.update(entity.step(world));
    if (entity.area.topLeft != result.back().first)
      result.emplace_back(entity.area.topLeft, world.time);
  }

  result.emplace_back(entity.area.topLeft, world.time);
  return result;
}
