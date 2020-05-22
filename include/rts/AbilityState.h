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
        const World& world, const EntitySPtr& entity, WorldActionList& actions) = 0;
  };

  template<typename Derived>
  class AbilityStateTpl : public AbilityState {
  public:
    using AbilityState::AbilityState;

    struct Create {
      AbilityStateUPtr operator()(Position target) const {
        return std::make_unique<Derived>(target);
      }
    };
  };
}
