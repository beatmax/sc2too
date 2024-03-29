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
#include <bitset>
#include <cassert>
#include <utility>

namespace rts {
  class Side {
  public:
    explicit Side(UnitTypeId baseType, const ResourceInitList& resources, UiUPtr ui)
      : baseType_{baseType}, resources_{resources}, ui_{std::move(ui)} {}

    WorldActionList exec(const World& w, const Command& cmd) __attribute__((warn_unused_result));
    UnitTypeId baseType() const { return baseType_; }
    Selection& selection() { return selection_; }
    const Selection& selection() const { return selection_; }
    const Group& group(ControlGroupId group) const { return groups_[group]; }
    ResourceBank& resources() { return resources_; }
    const ResourceBank& resources() const { return resources_; }
    ResourceAccount& resource(ResourceId r) { return resources_[r]; }
    const ResourceAccount& resource(ResourceId r) const { return resources_[r]; }
    MessageList& messages() { return messages_; }
    const MessageList& messages() const { return messages_; }
    void updateActiveUnitCount(UnitTypeId type, int16_t inc);
    Count activeUnitCount(UnitTypeId type) const { return activeUnitCounts_[type.idx]; }
    void addUpgrade(UpgradeId u);
    bool hasUpgrade(UpgradeId u) const { return upgrades_[u.idx]; }
    void setUpgradeInResearch(UpgradeId u, bool b) { upgradesInResearch_[u.idx] = b; }
    bool isUpgradeInResearch(UpgradeId u) const { return upgradesInResearch_[u.idx]; }
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
    AbilityInstanceIndex preparedAbilityIndex() const { return preparedAbilityIndex_; }
    Sequence triggerCount() const { return triggerCount_; }
    const Ui& ui() const { return *ui_; }

    void onMapCreated(World& w);
    void onUnitDestroyed(World& w);

  private:
    using ControlGroupArray = std::array<Selection, MaxControlGroups>;
    using UnitCountArray = std::array<Count, MaxUnitTypes + 1>;
    using UpgradeBitset = std::bitset<MaxUpgrades + 1>;

    void exec(const World& w, WorldActionList& actions, const command::Cancel& cmd);
    void exec(const World& w, WorldActionList& actions, const command::ControlGroup& cmd);
    void exec(const World& w, WorldActionList& actions, const command::PrepareAbility& cmd);
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
    UnitCountArray activeUnitCounts_{};
    UpgradeBitset upgrades_;
    UpgradeBitset upgradesInResearch_;
    PowerMap powerMap_;
    Map prototypeMap_;
    UnitId prototype_;
    UnitTypeId prototypeBuilderType_;
    AbilityInstanceIndex preparedAbilityIndex_;
    Sequence triggerCount_{};
    UiUPtr ui_;
  };
}
