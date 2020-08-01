#pragma once

#include "Command.h"
#include "types.h"

#include <iterator>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

namespace rts {

  namespace action {
    struct CommandAction {
      SideId sideId;
      Command command;
    };

    struct AbilityStepAction {
      EntityWId entityWId;
      EntityAbilityIndex abilityIndex;
    };
  }

  using WorldAction = std::variant<action::CommandAction, action::AbilityStepAction>;
  using WorldActionList = std::vector<WorldAction>;

  template<typename T>
  void addAction(WorldActionList& actions, T&& action) {
    actions.push_back(std::forward<T>(action));
  }

  inline void addActions(WorldActionList& actions, WorldActionList&& moreActions) {
    actions.insert(
        actions.end(), std::make_move_iterator(moreActions.begin()),
        std::make_move_iterator(moreActions.end()));
  }

  inline void addCommand(WorldActionList& actions, SideId side, std::optional<Command>&& cmd) {
    if (cmd)
      actions.push_back(action::CommandAction{side, std::move(*cmd)});
  }

}
