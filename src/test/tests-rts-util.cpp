#include "tests-rts-util.h"

#include "catch2/catch.hpp"
#include "rts/Command.h"
#include "rts/Entity.h"
#include "rts/World.h"
#include "tests-rts-assets.h"

#include <utility>

using namespace rts;

void test::execCommand(rts::World& w, rts::SideId side, rts::Command command) {
  w.exec({{side, std::move(command)}});
}

void test::select(
    rts::World& w,
    rts::SideId side,
    rts::EntityIdList entities,
    rts::command::Selection::Action action) {
  execCommand(w, side, rts::command::Selection{action, std::move(entities)});
}

test::MoveStepList test::runMove(World& w, Entity& entity, Point target, GameTime timeout) {
  execCommand(w, entity.side, rts::command::TriggerAbility{MoveAbilityId, target});
  return continueMove(w, entity, timeout);
}

test::MoveStepList test::continueMove(World& w, Entity& entity, GameTime timeout) {
  auto ai{w[entity.type].abilityIndex(MoveAbilityId)};
  REQUIRE(ai != EntityAbilityIndex::None);
  AbilityState& moveAbilityState{entity.abilityStates[ai]};

  MoveStepList result;
  result.emplace_back(entity.area.topLeft, w.time);

  while (moveAbilityState.active() && w.time < timeout) {
    ++w.time;
    w.update(entity.step(w));
    if (entity.area.topLeft != result.back().first)
      result.emplace_back(entity.area.topLeft, w.time);
  }

  if (MoveStep{entity.area.topLeft, w.time} != result.back())
    result.emplace_back(entity.area.topLeft, w.time);
  return result;
}
