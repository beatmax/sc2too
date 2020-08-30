#pragma once

#include "Command.h"
#include "Message.h"
#include "Resource.h"
#include "Selection.h"
#include "WorldAction.h"
#include "constants.h"
#include "types.h"

#include <array>
#include <cassert>
#include <utility>

namespace rts {
  class Side {
  public:
    explicit Side(const ResourceInitList& resources, UiUPtr ui)
      : resources_{resources}, ui_{std::move(ui)} {}

    WorldActionList exec(const World& w, const Command& cmd) __attribute__((warn_unused_result));
    Selection& selection() { return selection_; }
    const Selection& selection() const { return selection_; }
    const Group& group(ControlGroupId group) const { return groups_[group]; }
    ResourceBank& resources() { return resources_; }
    const ResourceBank& resources() const { return resources_; }
    ResourceAccount& resource(ResourceId r) { return resources_[r]; }
    const ResourceAccount& resource(ResourceId r) const { return resources_[r]; }
    MessageList& messages() { return messages_; }
    const MessageList& messages() const { return messages_; }
    const Ui& ui() const { return *ui_; }

  private:
    using ControlGroupArray = std::array<Selection, MaxControlGroups>;

    void exec(const World& w, WorldActionList& actions, const command::ControlGroup& cmd);
    void exec(const World& w, WorldActionList& actions, const command::Selection& cmd);
    void exec(const World& w, WorldActionList& actions, const command::SelectionSubgroup& cmd);
    void exec(const World& w, WorldActionList& actions, const command::TriggerAbility& cmd);
    void exec(const World& w, WorldActionList& actions, const command::TriggerDefaultAbility& cmd);
    void exec(const World& w, WorldActionList& actions, const command::Debug& cmd);

    ResourceBank resources_;
    Selection selection_;
    ControlGroupArray groups_;
    MessageList messages_;
    UiUPtr ui_;
  };
}
