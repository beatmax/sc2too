#pragma once

#include "types.h"
#include "util/functional.h"

#include <utility>
#include <vector>

namespace rts {

  using WorldAction = std::function<void(World&)>;
  using WorldActionList = std::vector<WorldAction>;

  template<typename F, typename... Args>
  void addAction(WorldActionList& actions, F f, Args&&... args) {
    actions.push_back(util::Binder<F>{}(f, std::forward<Args>(args)...));
  }

}
