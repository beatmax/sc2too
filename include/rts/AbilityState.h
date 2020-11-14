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

    void trigger(
        World& w,
        Unit& u,
        TriggerGroup& group,
        const abilities::Instance& ai,
        const AbilityTarget& target);
    void step(const World& w, UnitStableRef u, AbilityStateIndex as, WorldActionList& actions);
    void stepAction(World& w, Unit& u, AbilityStateIndex as, const AbilityStepAction& f);
    void cancel(World& w, Unit& u, AbilityStateIndex as);

    template<typename T>
    std::optional<T> state() const;

  private:
    void destroyActiveState(Unit& u, AbilityStateIndex as);

    GameTime nextStepTime_{GameTimeInf};
    ActiveAbilityStateUPtr activeState_;
  };

  class ActiveAbilityState {
  public:
    virtual ~ActiveAbilityState() = 0;
    virtual AbilityStepResult step(const World& w, UnitStableRef u) = 0;
    virtual void cancel(World& w) = 0;
    virtual int state() const = 0;
  };

  class StatelessAbilityState {};

  template<typename Derived>
  class ActiveAbilityStateTpl : public ActiveAbilityState {
  public:
    template<typename D>
    static auto makeTrigger(const D& desc) {
      return [desc](
                 World& w, Unit& u, TriggerGroup&, ActiveAbilityStateUPtr& as,
                 const AbilityTarget& target) -> std::pair<ActiveAbilityStateUPtr, GameTime> {
        return {Derived::trigger(w, u, as, desc, target), 1};
      };
    }
  };

  template<typename Derived>
  class ActiveAbilityGroupStateTpl : public ActiveAbilityState {
  public:
    template<typename D>
    static auto makeTrigger(const D& desc) {
      return [desc](
                 World& w, Unit& u, TriggerGroup& tg, ActiveAbilityStateUPtr& as,
                 const AbilityTarget& target) -> std::pair<ActiveAbilityStateUPtr, GameTime> {
        return Derived::trigger(w, u, tg, as, desc, target);
      };
    }
  };

  template<typename Derived>
  class StatelessAbilityStateTpl : public StatelessAbilityState {
  public:
    template<typename D>
    static auto makeTrigger(const D& desc) {
      return [desc](
                 World& w, Unit& u, TriggerGroup&, ActiveAbilityStateUPtr&,
                 const AbilityTarget& target) -> std::pair<ActiveAbilityStateUPtr, GameTime> {
        Derived::trigger(w, u, desc, target);
        return {};
      };
    }
  };

  class ActiveAbilityGroupState {};
}

template<typename T>
std::optional<T> rts::AbilityState::state() const {
  if (activeState_)
    return T{activeState_->state()};
  return std::nullopt;
}
