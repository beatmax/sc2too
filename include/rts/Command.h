#pragma once

#include "types.h"

#include <variant>

namespace rts {

  namespace command {
    struct TriggerAbility {
      AbilityId ability;
      Point target;
    };

    struct TriggerDefaultAbility {
      Point target;
    };

    struct Selection {
      enum { Set, Add, Remove } action;
      EntityIdList entities;
    };
  }

  using Command =
      std::variant<command::TriggerAbility, command::TriggerDefaultAbility, command::Selection>;
}
