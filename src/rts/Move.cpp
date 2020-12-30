#include "Move.h"

#include "rts/Unit.h"
#include "rts/World.h"
#include "rts/WorldAction.h"
#include "rts/constants.h"
#include "util/geo.h"

#include <cassert>
#include <cmath>
#include <tuple>

namespace rts {
  namespace {
    constexpr GameTime WaitingRetryTime{10};
    constexpr int WaitingMaxTries{10};

    inline Distance adjacentDistance(Point p1, Point p2) {
      return (p1.x == p2.x || p1.y == p2.y) ? CellDistance : DiagonalDistance;
    }

    inline float diagonalDistance(const World& w, Point p, const AbilityTarget& target) {
      return util::geo::diagonalDistance(p, w.center(target));
    }
  }
}

std::pair<rts::ActiveAbilityStateUPtr, rts::GameTime> rts::abilities::state::Move::trigger(
    World& w,
    Unit& u,
    TriggerGroup& group,
    ActiveAbilityStateUPtr& as,
    const Desc& desc,
    const AbilityTarget& target) {
  if (!group.sharedState) {
    auto sharedState = std::make_shared<GroupState>();
    sharedState->closeEnough = sqrtf(group.originalSize);
    group.sharedState = sharedState;
  }
  auto groupState = std::static_pointer_cast<GroupState>(group.sharedState);
  GameTime firstStepTime(++groupState->count);
  return {std::make_unique<Move>(desc, target, groupState), firstStepTime};
}

rts::AbilityStepResult rts::abilities::state::Move::step(const World& w, UnitStableRef u) {
  const Point pos{u->area.topLeft};

  if (!path_.empty()) {
    auto nextPoint{path_.front()};
    const auto& nextCell{w[nextPoint]};
    if (nextCell.empty() || dodge(w, pos, path_))
      return stepAction();

    if (nextCell.contains(Cell::Unit)) {
      auto cuId{nextCell.unitId()};
      const auto& cu{w[cuId]};
      if (cu.isActive(w, rts::abilities::Kind::Move)) {
        std::pair<Point, UnitId> pointUnit{nextPoint, cuId};
        if (waitingFor_ != pointUnit) {
          waitingFor_ = pointUnit;
          waitingTries = 0;
        }
        if (++waitingTries <= WaitingMaxTries)
          return WaitingRetryTime;
      }
      if (!cu.isStructure(w))
        excludedPoints_.insert(nextPoint);
    }
  }

  if (havePath_ && (path_.empty() || diagonalDistance(w, pos, target_) <= groupState_->closeEnough))
    return GameTime{0};

  bool complete;
  std::tie(path_, complete) = findPathToTarget(w, pos, target_, excludedPoints_);

  if (path_.empty())
    return GameTime{0};

  if (!complete)
    target_ = path_.back();

  if (havePath_)
    return GameTime{1};

  havePath_ = true;
  return adjacentDistance(pos, path_.front()) / desc_.speed;
}

rts::AbilityStepAction rts::abilities::state::Move::stepAction() {
  return [this](World& w, Unit& u) -> GameTime {
    assert(!path_.empty());

    Point newPos{path_.front()};
    if (w[newPos].empty()) {
      path_.pop_front();
      w.move(u, newPos);

      if (path_.empty())
        return GameTime{0};
      else
        return adjacentDistance(newPos, path_.front()) / desc_.speed;
    }
    else {
      return 1;
    }
  };
}
