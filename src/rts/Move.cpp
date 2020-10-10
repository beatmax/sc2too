#include "Move.h"

#include "rts/Unit.h"
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

void rts::abilities::state::Move::trigger(
    World& w, Unit& u, ActiveAbilityStateUPtr& as, const Desc& desc, const AbilityTarget& target) {
  if (as)
    as->cancel(w);
  as = std::make_unique<Move>(desc, target);
}

rts::AbilityStepResult rts::abilities::state::Move::step(const World& w, UnitStableRef u) {
  const Point& pos{u->area.topLeft};

  if (!path_.empty() && w[path_.front()].empty())
    return stepAction();

  if (path_.empty() && completePath_)
    return GameTime{0};

  const bool wasEmpty{path_.empty()};
  std::tie(path_, completePath_) = findPathToTarget(w, pos, target_);

  return (wasEmpty && !path_.empty()) ? adjacentDistance(pos, path_.front()) / desc_.speed
                                      : (path_.empty() && !completePath_) ? MoveRetryTime : 1;
}

rts::AbilityStepAction rts::abilities::state::Move::stepAction() {
  return [this](World& w, Unit& u) -> GameTime {
    assert(!path_.empty());
    Point newPos{path_.front()};
    if (w[newPos].empty()) {
      path_.pop_front();
      w.move(u, newPos);
      if (path_.empty())
        return completePath_ ? 0 : 1;
      else
        return adjacentDistance(newPos, path_.front()) / desc_.speed;
    }
    else {
      return 1;
    }
  };
}
