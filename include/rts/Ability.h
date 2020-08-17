#pragma once

#include "types.h"

#include <memory>
#include <utility>

namespace rts {
  class ActiveAbilityState;
  using ActiveAbilityStateUPtr = std::unique_ptr<ActiveAbilityState>;

  class Ability {
  public:
    enum class TargetType { None, Any, Resource };

    const TargetType targetType;
    UiUPtr ui;

    Ability(TargetType tt, UiUPtr ui) : targetType{tt}, ui{std::move(ui)} {}
  };
}
