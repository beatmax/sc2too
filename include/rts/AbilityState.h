#pragma once

#include "Ability.h"
#include "WorldAction.h"
#include "constants.h"
#include "types.h"

#include <memory>
#include <optional>

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
        const World& w,
        const Entity& entity,
        EntityAbilityIndex abilityIndex,
        WorldActionList& actions);
    void stepAction(World& w, Entity& entity);
    void cancel(const World& w, WorldActionList& actions);

    template<typename T>
    std::optional<T> state() const;

  private:
    GameTime nextStepTime_{GameTimeInf};
    ActiveAbilityStateUPtr activeState_;
  };

  class ActiveAbilityState {
  public:
    virtual ~ActiveAbilityState() = 0;
    virtual GameTime step(
        const World& w,
        const Entity& entity,
        EntityAbilityIndex abilityIndex,
        WorldActionList& actions) = 0;
    virtual GameTime stepAction(World& w, Entity& entity) = 0;
    virtual void cancel(const World& w, WorldActionList& actions) = 0;
    virtual int state() const = 0;
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

template<typename T>
std::optional<T> rts::AbilityState::state() const {
  if (activeState_)
    return T{activeState_->state()};
  return std::nullopt;
}
