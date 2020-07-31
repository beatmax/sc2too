#include "tests-rts-util.h"

#include "catch2/catch.hpp"
#include "rts/Entity.h"
#include "rts/World.h"
#include "tests-rts-assets.h"

using namespace rts;

test::MoveStepList test::runMove(World& world, Entity& entity, Point target, GameTime timeout) {
  world.update(entity.trigger(MoveAbilityId, world, target));
  return continueMove(world, entity, timeout);
}

test::MoveStepList test::continueMove(World& world, Entity& entity, GameTime timeout) {
  auto ai{world.entityTypes[entity.type].abilityIndex(MoveAbilityId)};
  REQUIRE(ai != EntityAbilityIndex::None);
  AbilityState& moveAbilityState{entity.abilityStates[ai]};

  MoveStepList result;
  result.emplace_back(entity.area.topLeft, world.time);

  while (moveAbilityState.active() && world.time < timeout) {
    ++world.time;
    world.update(entity.step(world));
    if (entity.area.topLeft != result.back().first)
      result.emplace_back(entity.area.topLeft, world.time);
  }

  if (MoveStep{entity.area.topLeft, world.time} != result.back())
    result.emplace_back(entity.area.topLeft, world.time);
  return result;
}
