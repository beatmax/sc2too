#include "tests-rts-util.h"

#include "catch2/catch.hpp"
#include "rts/Command.h"
#include "rts/Unit.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "tests-rts-assets.h"

#include <utility>

using namespace rts;

void test::execCommand(World& w, SideId side, Command command) {
  w.exec({{side, std::move(command)}});
}

void test::select(World& w, SideId side, UnitIdList units, command::Selection::Action action) {
  execCommand(w, side, command::Selection{action, std::move(units)});
}

test::MoveStepList test::runMove(World& w, Unit& unit, Point target, GameTime timeout) {
  execCommand(w, unit.side, command::TriggerAbility{moveAbilityId, target});
  return continueMove(w, unit, timeout);
}

test::MoveStepList test::continueMove(World& w, Unit& unit, GameTime timeout) {
  const AbilityState& moveAbilityState{unit.abilityState(w, abilities::Kind::Move)};

  MoveStepList result;
  result.emplace_back(unit.area.topLeft, w.time);

  while (moveAbilityState.active() && w.time < timeout) {
    ++w.time;
    w.update(unit.step(w));
    if (unit.area.topLeft != result.back().first)
      result.emplace_back(unit.area.topLeft, w.time);
  }

  if (MoveStep{unit.area.topLeft, w.time} != result.back())
    result.emplace_back(unit.area.topLeft, w.time);
  return result;
}
