#include "tests-rts-util.h"

#include "catch2/catch.hpp"
#include "rts/Command.h"
#include "rts/Side.h"
#include "rts/Unit.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "tests-rts-assets.h"

#include <utility>

using namespace rts;

void test::TestAccount::allocate(rts::Quantity q) {
  available -= q;
  allocated += q;
}

bool test::TestAccount::operator==(const TestAccount& other) const {
  return available == other.available && allocated == other.allocated;
}

test::TestResources::TestResources(const rts::Side& s) {
  auto& sGas{s.resources()[gasResourceId]};
  auto& sSupply{s.resources()[supplyResourceId]};
  gas.available = sGas.available();
  gas.allocated = sGas.allocated();
  supply.available = sSupply.available();
  supply.allocated = sSupply.allocated();
}

bool test::TestResources::operator==(const TestResources& other) const {
  return gas == other.gas && supply == other.supply;
}

std::ostream& test::operator<<(std::ostream& os, const TestAccount& a) {
  return os << "available=" << a.available << " allocated=" << a.allocated;
}

std::ostream& test::operator<<(std::ostream& os, const TestResources& r) {
  return os << "gas:{" << r.gas << "}, supply:{" << r.supply << "}";
}

GameTime test::nextStepTime(const Unit& u) {
  return Unit::nextStepTime(UnitStableRef{u});
}

void test::stepUpdate(World& w, const Unit& u) {
  w.update(Unit::step(UnitStableRef{u}, w));
}

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
  const AbilityState& moveAbilityState{
      Unit::abilityState(UnitStableRef{unit}, w, abilities::Kind::Move)};

  MoveStepList result;
  result.emplace_back(unit.area.topLeft, w.time);

  while (moveAbilityState.active() && w.time < timeout) {
    ++w.time;
    stepUpdate(w, unit);
    if (unit.area.topLeft != result.back().first)
      result.emplace_back(unit.area.topLeft, w.time);
  }

  if (MoveStep{unit.area.topLeft, w.time} != result.back())
    result.emplace_back(unit.area.topLeft, w.time);
  return result;
}
