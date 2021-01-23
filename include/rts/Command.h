#pragma once

#include "types.h"

#include <variant>

namespace rts {

  namespace command {
    struct BuildPrototype {
      UnitTypeId unitType;
    };

    struct Cancel {};

    struct ControlGroup {
      enum Action { Select, Set, Add } action;
      bool exclusive;
      ControlGroupId group;
    };

    struct Selection {
      enum Action { Set, Add, Remove } action;
      UnitIdList units;
    };

    struct SelectionSubgroup {
      enum Action { Next, Previous } action;
    };

    struct TriggerAbility {
      AbilityInstanceIndex abilityIndex;
      Point target;
      bool enqueue{false};
    };

    struct TriggerDefaultAbility {
      Point target;
      bool enqueue{false};
      bool minimap{false};
    };

    struct Debug {
      enum Action { Destroy } action;
    };
  }

  using Command = std::variant<
      command::BuildPrototype,
      command::Cancel,
      command::ControlGroup,
      command::Selection,
      command::SelectionSubgroup,
      command::TriggerAbility,
      command::TriggerDefaultAbility,
      command::Debug>;

  struct SideCommand {
    SideId side;
    Command command;
  };
}
