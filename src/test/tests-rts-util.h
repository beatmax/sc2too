#pragma once

#include "rts/Command.h"
#include "rts/types.h"

#include <utility>
#include <vector>

namespace test {
  using MoveStep = std::pair<rts::Point, rts::GameTime>;
  using MoveStepList = std::vector<MoveStep>;

  void execCommand(rts::World& w, rts::SideId side, rts::Command command);
  void select(rts::World& w, rts::SideId side, rts::EntityIdList entities);
  MoveStepList runMove(
      rts::World& w, rts::Entity& entity, rts::Point target, rts::GameTime timeout = 99999);
  MoveStepList continueMove(rts::World& w, rts::Entity& entity, rts::GameTime timeout = 99999);
}
