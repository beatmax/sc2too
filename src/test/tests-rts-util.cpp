#include "tests-rts-util.h"

#include "catch2/catch.hpp"
#include "rts/Command.h"
#include "rts/Entity.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "tests-rts-assets.h"

#include <utility>

using namespace rts;

void test::execCommand(World& w, SideId side, Command command) {
  w.exec({{side, std::move(command)}});
}

void test::select(World& w, SideId side, EntityIdList entities, command::Selection::Action action) {
  execCommand(w, side, command::Selection{action, std::move(entities)});
}

test::MoveStepList test::runMove(World& w, Entity& entity, Point target, GameTime timeout) {
  execCommand(w, entity.side, command::TriggerAbility{MoveAbilityId, target});
  return continueMove(w, entity, timeout);
}

test::MoveStepList test::continueMove(World& w, Entity& entity, GameTime timeout) {
  const AbilityState& moveAbilityState{entity.abilityState(w, abilities::Kind::Move)};

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
