#pragma once

#include "types.h"
#include "util/functional.h"

#include <iterator>
#include <utility>
#include <vector>

namespace rts {

  using WorldAction = std::function<void(World&)>;
  using WorldActionList = std::vector<WorldAction>;

  template<typename F, typename... Args>
  void addAction(WorldActionList& actions, F f, Args&&... args) {
    actions.push_back(util::Binder<F>{}(f, std::forward<Args>(args)...));
  }

  inline void addActions(WorldActionList& actions, WorldActionList&& moreActions) {
    actions.insert(
        actions.end(), std::make_move_iterator(moreActions.begin()),
        std::make_move_iterator(moreActions.end()));
  }

}
