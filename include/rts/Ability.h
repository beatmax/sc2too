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
    explicit Ability(UiUPtr ui) : ui_{std::move(ui)} {}
    const Ui& ui() const { return *ui_; }

  private:
    UiUPtr ui_;
  };

  struct AbilityInstance {
    using CreateActiveState = std::function<ActiveAbilityStateUPtr(Point)>;

    AbilityId abilityId;
    CreateActiveState createActiveState;
  };
}
