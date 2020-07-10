#pragma once

#include "rts/types.h"

#include <utility>
#include <vector>

namespace test {
  using MoveStep = std::pair<rts::Point, rts::GameTime>;
  using MoveStepList = std::vector<MoveStep>;

  MoveStepList runMove(
      rts::World& world, rts::Entity& entity, rts::Point target, rts::GameTime timeout = 99999);
  MoveStepList continueMove(rts::World& world, rts::Entity& entity, rts::GameTime timeout = 99999);
}
