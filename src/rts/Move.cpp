#include "Move.h"

#include "rts/Entity.h"
#include "rts/World.h"
#include "rts/WorldAction.h"
#include "rts/constants.h"

#include <cassert>
#include <tuple>

namespace rts {
  namespace {
    constexpr GameTime MoveRetryTime{100};

    inline Distance adjacentDistance(const Point& p1, const Point& p2) {
      return (p1.x == p2.x || p1.y == p2.y) ? CellDistance : DiagonalDistance;
    }
  }
}

rts::GameTime rts::abilities::state::Move::step(
    const World& w,
    const Entity& entity,
    EntityAbilityIndex abilityIndex,
    WorldActionList& actions) {
  const Point& pos{entity.area.topLeft};

  if (!path_.empty() && w[path_.front()].empty()) {
    addStepAction(w, entity, abilityIndex, actions);
    return GameTimeInf;
  }

  if (path_.empty() && completePath_)
    return 0;

  const bool wasEmpty{path_.empty()};
  std::tie(path_, completePath_) = findPath(w, pos, target_);

  return (wasEmpty && !path_.empty()) ? adjacentDistance(pos, path_.front()) / speed_
                                      : (path_.empty() && !completePath_) ? MoveRetryTime : 1;
}

rts::GameTime rts::abilities::state::Move::stepAction(World& w, Entity& entity) {
  assert(!path_.empty());
  Point newPos{path_.front()};
  if (w[newPos].empty()) {
    path_.pop_front();
    w.move(entity, newPos);
    if (path_.empty())
      return completePath_ ? 0 : 1;
    else
      return adjacentDistance(newPos, path_.front()) / speed_;
  }
  else {
    return 1;
  }
}
