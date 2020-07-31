#pragma once

#include "Ability.h"
#include "WorldAction.h"
#include "dimensions.h"
#include "types.h"

#include <memory>

namespace rts {
  class ActiveAbilityState;
  using ActiveAbilityStateUPtr = std::unique_ptr<ActiveAbilityState>;

  class AbilityState {
  public:
    bool active() const { return bool(activeState_); }
    GameTime nextStepTime() const { return nextStepTime_; }

    void trigger(const AbilityInstance& ability, Point target) {
      activeState_ = ability.createActiveState(target);
    }
    void step(
        const World& world,
        const Entity& entity,
        EntityAbilityIndex abilityIndex,
        WorldActionList& actions);
    void stepAction(World& world, Entity& entity);
    void cancel();

  private:
    GameTime nextStepTime_{GameTimeInf};
    ActiveAbilityStateUPtr activeState_;
  };

  class ActiveAbilityState {
  public:
    virtual ~ActiveAbilityState() = 0;
    virtual GameTime step(
        const World& world,
        const Entity& entity,
        EntityAbilityIndex abilityIndex,
        WorldActionList& actions) = 0;
    virtual GameTime stepAction(World& world, Entity& entity) = 0;
  };

  template<typename Derived>
  class ActiveAbilityStateTpl : public ActiveAbilityState {
  public:
    using ActiveAbilityState::ActiveAbilityState;

    template<typename... Args>
    static auto creator(Args... args) {
      return [=](Point target) { return std::make_unique<Derived>(target, args...); };
    }
  };
}
