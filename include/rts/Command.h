#pragma once

#include "types.h"

#include <variant>

namespace rts {

  namespace command {
    struct ControlGroup {
      enum { Select, Set, Add } action;
      bool exclusive;
      ControlGroupId group;
    };

    struct Selection {
      enum { Set, Add, Remove } action;
      EntityIdList entities;
    };

    struct TriggerAbility {
      AbilityId ability;
      Point target;
    };

    struct TriggerDefaultAbility {
      Point target;
    };
  }

  using Command = std::variant<
      command::ControlGroup,
      command::Selection,
      command::TriggerAbility,
      command::TriggerDefaultAbility>;
}
