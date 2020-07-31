#include "rts/abilities.h"

#include "rts/AbilityState.h"
#include "rts/Entity.h"
#include "rts/Path.h"
#include "rts/World.h"
#include "rts/dimensions.h"
#include "rts/types.h"

#include <cassert>
#include <tuple>

namespace rts {
  namespace {
    constexpr GameTime MoveRetryTime{100};

    void addStepAction(
        const World& world,
        const Entity& entity,
        EntityAbilityIndex abilityIndex,
        WorldActionList& actions) {
      addAction(actions, action::AbilityStepAction{world.entities.weakId(entity), abilityIndex});
    }
  }
}

namespace rts::abilities::state {

  class Move : public ActiveAbilityStateTpl<Move> {
  public:
    explicit Move(Point target, Speed speed) : target_{target}, speed_{speed} {}
    GameTime step(
        const World& world,
        const Entity& entity,
        EntityAbilityIndex abilityIndex,
        WorldActionList& actions) final;
    GameTime stepAction(World& world, Entity& entity) final;

  private:
    const Point target_;
    Speed speed_;
    Path path_;
    bool completePath_{false};
  };
}

rts::GameTime rts::abilities::state::Move::step(
    const World& world,
    const Entity& entity,
    EntityAbilityIndex abilityIndex,
    WorldActionList& actions) {
  const Point& pos{entity.area.topLeft};

  if (!path_.empty() && isFree(world.map.at(path_.front()))) {
    addStepAction(world, entity, abilityIndex, actions);
    return GameTimeInf;
  }

  if (path_.empty() && completePath_)
    return 0;

  const bool wasEmpty{path_.empty()};
  std::tie(path_, completePath_) = findPath(world, pos, target_);

  return (wasEmpty && !path_.empty()) ? adjacentDistance(pos, path_.front()) / speed_
                                      : (path_.empty() && !completePath_) ? MoveRetryTime : 1;
}

rts::GameTime rts::abilities::state::Move::stepAction(World& world, Entity& entity) {
  assert(!path_.empty());
  Point newPos{path_.front()};
  if (isFree(world.map.at(newPos))) {
    path_.pop_front();
    world.move(entity, newPos);
    if (path_.empty())
      return completePath_ ? 0 : 1;
    else
      return adjacentDistance(newPos, path_.front()) / speed_;
  }
  else {
    return 1;
  }
}

rts::AbilityInstance rts::abilities::move(AbilityId ability, Speed speed) {
  return AbilityInstance{ability, state::Move::creator(speed)};
}
