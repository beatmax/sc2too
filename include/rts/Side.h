#pragma once

#include "Command.h"
#include "Map.h"
#include "Message.h"
#include "PowerMap.h"
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
    explicit Side(UnitTypeId baseType, const ResourceInitList& resources, UiUPtr ui)
      : baseType_{baseType}, resources_{resources}, ui_{std::move(ui)} {}

    WorldActionList exec(const World& w, const Command& cmd) __attribute__((warn_unused_result));
    const UnitTypeId baseType() const { return baseType_; }
    Selection& selection() { return selection_; }
    const Selection& selection() const { return selection_; }
    const Group& group(ControlGroupId group) const { return groups_[group]; }
    ResourceBank& resources() { return resources_; }
    const ResourceBank& resources() const { return resources_; }
    ResourceAccount& resource(ResourceId r) { return resources_[r]; }
    const ResourceAccount& resource(ResourceId r) const { return resources_[r]; }
    MessageList& messages() { return messages_; }
    const MessageList& messages() const { return messages_; }
    PowerMap& powerMap() { return powerMap_; }
    const PowerMap& powerMap() const { return powerMap_; }
    Map& prototypeMap() { return prototypeMap_; }
    const Map& prototypeMap() const { return prototypeMap_; }
    void createPrototype(World& w, UnitTypeId t, UnitTypeId builderType);
    void destroyPrototype(World& w);
    bool materializePrototype(World& w, Point p);
    UnitId takePrototype();
    UnitId takeAndCreatePrototype(World& w);
    UnitId prototype() const { return prototype_; }
    const Ui& ui() const { return *ui_; }

    void onMapCreated(World& w);
    void onUnitDestroyed(World& w);

  private:
    using ControlGroupArray = std::array<Selection, MaxControlGroups>;

    void exec(const World& w, WorldActionList& actions, const command::BuildPrototype& cmd);
    void exec(const World& w, WorldActionList& actions, const command::Cancel& cmd);
    void exec(const World& w, WorldActionList& actions, const command::ControlGroup& cmd);
    void exec(const World& w, WorldActionList& actions, const command::Selection& cmd);
    void exec(const World& w, WorldActionList& actions, const command::SelectionSubgroup& cmd);
    void exec(const World& w, WorldActionList& actions, const command::TriggerAbility& cmd);
    void exec(const World& w, WorldActionList& actions, const command::TriggerDefaultAbility& cmd);
    void exec(const World& w, WorldActionList& actions, const command::Debug& cmd);

    UnitTypeId baseType_;
    ResourceBank resources_;
    Selection selection_;
    ControlGroupArray groups_;
    MessageList messages_;
    PowerMap powerMap_;
    Map prototypeMap_;
    UnitId prototype_;
    UnitTypeId prototypeBuilderType_;
    UiUPtr ui_;
  };
}
