#pragma once

#include "types.h"

#include <functional>
#include <memory>
#include <utility>

namespace rts {
  class ActiveAbilityState;
  using ActiveAbilityStateUPtr = std::unique_ptr<ActiveAbilityState>;

  class Ability {
  public:
    UiUPtr ui;

    explicit Ability(UiUPtr ui) : ui{std::move(ui)} {}
  };

  struct AbilityInstance {
    using CreateActiveState = std::function<ActiveAbilityStateUPtr(Point)>;

    AbilityId abilityId;
    CreateActiveState createActiveState;
  };
}
