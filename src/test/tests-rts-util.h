#pragma once

#include "rts/Command.h"
#include "rts/types.h"

#include <utility>
#include <vector>

namespace test {
  using MoveStep = std::pair<rts::Point, rts::GameTime>;
  using MoveStepList = std::vector<MoveStep>;

  rts::GameTime nextStepTime(const rts::Unit& u);
  void stepUpdate(rts::World& w, const rts::Unit& u);
  void execCommand(rts::World& w, rts::SideId side, rts::Command command);
  void select(
      rts::World& w,
      rts::SideId side,
      rts::UnitIdList units,
      rts::command::Selection::Action action = rts::command::Selection::Set);
  MoveStepList runMove(
      rts::World& w, rts::Unit& unit, rts::Point target, rts::GameTime timeout = 99999);
  MoveStepList continueMove(rts::World& w, rts::Unit& unit, rts::GameTime timeout = 99999);
}
