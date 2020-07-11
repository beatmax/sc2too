#pragma once

#include "WorldAction.h"
#include "types.h"

#include <memory>

namespace rts {
  class AbilityState;
  using AbilityStateUPtr = std::unique_ptr<AbilityState>;

  class AbilityState {
  public:
    virtual ~AbilityState() = 0;
    virtual GameTime step(
        const World& world,
        const Entity& entity,
        const Ability& ability,
        WorldActionList& actions) = 0;
    virtual GameTime stepAction(World& world, Entity& entity, Ability& ability) = 0;
  };

  template<typename Derived>
  class AbilityStateTpl : public AbilityState {
  public:
    using AbilityState::AbilityState;

    template<typename... Args>
    static auto creator(Args... args) {
      return [=](Point target) { return std::make_unique<Derived>(target, args...); };
    }
  };
}
