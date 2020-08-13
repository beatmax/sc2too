#pragma once

#include "types.h"

#include <variant>

namespace rts {

  namespace command {
    struct ControlGroup {
      enum Action { Select, Set, Add } action;
      bool exclusive;
      ControlGroupId group;
    };

    struct Selection {
      enum Action { Set, Add, Remove } action;
      EntityIdList entities;
    };

    struct SelectionSubgroup {
      enum Action { Next, Previous } action;
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
      command::SelectionSubgroup,
      command::TriggerAbility,
      command::TriggerDefaultAbility>;
}
