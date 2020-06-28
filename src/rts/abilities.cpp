#include "rts/abilities.h"

#include "rts/Entity.h"
#include "rts/World.h"
#include "rts/WorldAction.h"
#include "rts/types.h"

namespace rts::abilities::state {

  class Move : public AbilityStateTpl<Move> {
  public:
    explicit Move(Point target, Speed speed) : target_{target}, speed_{speed} {}

    GameTime step(const World& world, const Entity& entity, WorldActionList& actions) override {
      if (entity.area.contains(target_))
        return 0;
      else if (init_)
        addAction(actions, &World::moveTowards, target_, world.entities.weakId(entity));
      else
        init_ = true;
      return CellDistance / speed_;
    }

  private:
    Point target_;
    Speed speed_;
    bool init_{false};
  };
}

namespace ns = rts::abilities;

rts::Ability ns::move(Speed speed) {
  return Ability{"move", state::Move::creator(speed)};
}
