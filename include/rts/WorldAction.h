#pragma once

#include "types.h"

#include <functional>
#include <iterator>
#include <utility>
#include <vector>

namespace rts {
  using WorldAction = std::function<void(World&)>;
  using WorldActionList = std::vector<WorldAction>;

  template<typename F>
  WorldActionList& operator+=(WorldActionList& actions, F&& f) {
    actions.push_back(std::move(f));
    return actions;
  }

  inline WorldActionList& operator+=(WorldActionList& actions, WorldActionList&& more) {
    actions.insert(
        actions.end(), std::make_move_iterator(more.begin()), std::make_move_iterator(more.end()));
    return actions;
  }
}
