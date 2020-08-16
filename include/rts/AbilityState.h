#pragma once

#include "Ability.h"
#include "WorldAction.h"
#include "constants.h"
#include "types.h"

#include <functional>
#include <memory>
#include <optional>
#include <variant>

namespace rts {
  using AbilityStepAction = std::function<GameTime(World& w, Entity& e)>;
  using AbilityStepResult = std::variant<GameTime, AbilityStepAction>;

  class ActiveAbilityState;
  using ActiveAbilityStateUPtr = std::unique_ptr<ActiveAbilityState>;

  class AbilityState {
  public:
    bool active() const { return bool(activeState_); }
    GameTime nextStepTime() const { return nextStepTime_; }

    void trigger(World& w, const AbilityInstance& ability, Point target);
    void step(
        const World& w,
        const Entity& entity,
        EntityAbilityIndex abilityIndex,
        WorldActionList& actions);
    void stepAction(World& w, Entity& e, const AbilityStepAction& f);
    void cancel(World& w);

    template<typename T>
    std::optional<T> state() const;

  private:
    GameTime nextStepTime_{GameTimeInf};
    ActiveAbilityStateUPtr activeState_;
  };

  class ActiveAbilityState {
  public:
    virtual ~ActiveAbilityState() = 0;
    virtual AbilityStepResult step(
        const World& w, const Entity& entity, EntityAbilityIndex abilityIndex) = 0;
    virtual void cancel(World& w) = 0;
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
