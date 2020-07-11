#pragma once

#include "types.h"

#include <iterator>
#include <utility>
#include <variant>
#include <vector>

namespace rts {

  namespace action {
    struct AbilityStepAction {
      EntityWId entityWId;
      AbilityId abilityId;
    };
  }

  using WorldAction = std::variant<action::AbilityStepAction>;
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

}
