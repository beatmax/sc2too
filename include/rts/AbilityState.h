#pragma once

#include "WorldAction.h"
#include "abilities.h"
#include "constants.h"
#include "types.h"

#include <functional>
#include <memory>
#include <optional>
#include <variant>

namespace rts {
  using AbilityStepAction = std::function<GameTime(World& w, Unit& u)>;
  using AbilityStepResult = std::variant<GameTime, AbilityStepAction>;

  class ActiveAbilityState;
  using ActiveAbilityStateUPtr = std::unique_ptr<ActiveAbilityState>;

  class AbilityState {
  public:
    bool active() const { return bool(activeState_); }
    GameTime nextStepTime() const { return nextStepTime_; }

    void trigger(World& w, Unit& u, const abilities::Instance& ai, Point target);
    void step(const World& w, const Unit& u, AbilityStateIndex as, WorldActionList& actions);
    void stepAction(World& w, Unit& u, const AbilityStepAction& f);
    void cancel(World& w);

    abilities::Kind kind() const;
    template<typename T>
    std::optional<T> state() const;

  private:
    GameTime nextStepTime_{GameTimeInf};
    ActiveAbilityStateUPtr activeState_;
  };

  class ActiveAbilityState {
  public:
    virtual ~ActiveAbilityState() = 0;
    virtual AbilityStepResult step(const World& w, const Unit& u) = 0;
    virtual void cancel(World& w) = 0;

    virtual abilities::Kind kind() const = 0;
    virtual int state() const = 0;
  };

  template<typename Derived>
  class ActiveAbilityStateTpl : public ActiveAbilityState {
  public:
    using ActiveAbilityState::ActiveAbilityState;

    abilities::Kind kind() const final { return Derived::Desc::kind; }

    template<typename D>
    static auto makeTrigger(const D& desc) {
      return [desc](World& w, Unit& u, ActiveAbilityStateUPtr& as, Point target) {
        Derived::trigger(w, u, as, desc, target);
      };
    }
  };
}

template<typename T>
std::optional<T> rts::AbilityState::state() const {
  if (activeState_)
    return T{activeState_->state()};
  return std::nullopt;
}
