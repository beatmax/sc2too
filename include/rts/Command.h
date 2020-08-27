#pragma once

#include "types.h"

#include <optional>
#include <variant>
#include <vector>

namespace rts {

  namespace command {
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
      AbilityId ability;
      Point target;
    };

    struct TriggerDefaultAbility {
      Point target;
    };

    struct Debug {
      enum Action { Destroy } action;
    };
  }

  using Command = std::variant<
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

  using SideCommandList = std::vector<SideCommand>;

  inline void addCommand(SideCommandList& commands, SideId side, std::optional<Command>&& cmd) {
    if (cmd)
      commands.push_back(SideCommand{side, std::move(*cmd)});
  }
}
