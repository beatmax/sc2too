#include "rts/abilities.h"

#include "rts/Entity.h"
#include "rts/World.h"
#include "rts/WorldAction.h"
#include "rts/dimensions.h"
#include "rts/types.h"

#include <tuple>

namespace rts {
  namespace {
    constexpr GameTime MoveRetryTime{100};
  }
}

namespace rts::abilities::state {

  class Move : public AbilityStateTpl<Move> {
  public:
    explicit Move(Point target, Speed speed) : target_{target}, speed_{speed} {}
    GameTime step(const World& world, const Entity& entity, WorldActionList& actions) override;

  private:
    const Point target_;
    Speed speed_;
    Path path_;
    bool completePath_{false};
  };
}

rts::GameTime rts::abilities::state::Move::step(
    const World& world, const Entity& entity, WorldActionList& actions) {
  const Point& pos{entity.area.topLeft};

  if (!path_.empty() && isFree(world.map.at(path_.front()))) {
    addAction(actions, &World::move, world.entities.weakId(entity), &path_);

    if (path_.size() > 1)
      return adjacentDistance(path_.front(), path_[1]) / speed_;
    else
      return 1;
  }

  if (path_.empty() && completePath_)
    return 0;

  const bool wasEmpty{path_.empty()};
  std::tie(path_, completePath_) = findPath(world, pos, target_);

  return (wasEmpty && !path_.empty()) ? adjacentDistance(pos, path_.front()) / speed_
                                      : (path_.empty() && !completePath_) ? MoveRetryTime : 1;
}

rts::Ability rts::abilities::move(Speed speed) {
  return Ability{"move", state::Move::creator(speed)};
}
