#pragma once

#include "Command.h"
#include "Resource.h"
#include "Selection.h"
#include "constants.h"
#include "types.h"

#include <array>
#include <cassert>
#include <utility>

namespace rts {
  class Side {
  public:
    explicit Side(ResourceMap resources, UiUPtr ui)
      : resources_{std::move(resources)}, ui_{std::move(ui)} {}

    void exec(const World& w, const Command& cmd);
    Selection& selection() { return selection_; }
    const Selection& selection() const { return selection_; }
    const Selection& group(ControlGroupId group) const { return groups_[group]; }
    const ResourceMap& resources() const { return resources_; }
    Quantity quantity(ResourceCPtr r) const {
      auto it = resources_.find(r);
      assert(it != resources_.end());
      return it->second;
    }
    const Ui& ui() const { return *ui_; }

  private:
    using ControlGroupArray = std::array<Selection, MaxControlGroups>;

    void exec(const World& w, const command::ControlGroup& cmd);
    void exec(const World& w, const command::Selection& cmd);
    void exec(const World& w, const command::TriggerAbility& cmd);
    void exec(const World& w, const command::TriggerDefaultAbility& cmd);

    ResourceMap resources_;
    Selection selection_;
    ControlGroupArray groups_;
    UiUPtr ui_;
  };
}
