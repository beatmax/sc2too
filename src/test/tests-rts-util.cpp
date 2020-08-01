#include "tests-rts-util.h"

#include "catch2/catch.hpp"
#include "rts/Command.h"
#include "rts/Entity.h"
#include "rts/World.h"
#include "rts/WorldAction.h"
#include "tests-rts-assets.h"

#include <utility>

using namespace rts;

void test::execCommand(rts::World& world, rts::SideId side, rts::Command command) {
  WorldActionList actions;
  addCommand(actions, side, std::move(command));
  world.update(actions);
}

void test::select(rts::World& world, rts::SideId side, rts::EntityIdList entities) {
  execCommand(
      world, side, rts::command::Selection{rts::command::Selection::Set, std::move(entities)});
}

test::MoveStepList test::runMove(World& world, Entity& entity, Point target, GameTime timeout) {
  execCommand(world, entity.side, rts::command::TriggerAbility{MoveAbilityId, target});
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
