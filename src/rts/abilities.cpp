#include "rts/abilities.h"

#include "rts/Entity.h"
#include "rts/World.h"
#include "rts/WorldAction.h"
#include "rts/types.h"

namespace rts::abilities::state {

  class Move : public AbilityStateTpl<Move> {
  public:
    explicit Move(Point target) : target_{target} {}
    GameTime step(const World& world, const EntitySPtr& entity, WorldActionList& actions) override {
      if (entity->area.contains(target_))
        return 0;
      else if (init_)
        addAction(actions, &World::moveTowards, target_, entity);
      else
        init_ = true;
      return 1;
    }

  private:
    Point target_;
    bool init_{false};
  };
}

namespace ns = rts::abilities;

rts::Ability ns::move() {
  return Ability{"move", state::Move::Create{}};
}
