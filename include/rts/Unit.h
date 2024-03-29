#pragma once

#include "AbilityState.h"
#include "ReferenceCount.h"
#include "Resource.h"
#include "WorldObject.h"
#include "constants.h"
#include "types.h"
#include "util/CircularBuffer.h"
#include "util/Pool.h"

#include <array>
#include <optional>
#include <utility>

namespace rts {
  class Unit : public WorldObject {
  public:
    enum class State { New, Allocated, Buildable, Building, Active, Destroyed };
    enum class CancelOthers { No, Yes };

    UnitTypeId type;
    SideId side;
    State state{State::New};
    Quantity energy{0};
    ResourceFlexBag bag;
    ProductionQueueId productionQueue;
    ResourceFieldId resourceField;
    ReferenceCount workerCount;
    std::array<bool, MaxUnitAbilityStates> abilityActive{};
    util::CircularBuffer<UnitCommand, MaxEnqueuedCommands> commandQueue;
    bool powered{true};

    Unit(
        Vector s,
        UnitTypeId t,
        SideId sd,
        UiUPtr ui,
        Quantity cargoCapacity = 0,
        ProductionQueueId pq = {},
        ResourceFieldId rf = {});
    void allocate(World& w, bool allocCheck = true);
    bool tryAllocate(World& w);
    void setBuildPoint(World& w, Point p);
    bool tryStartBuilding(World& w);
    void finishBuilding(World& w);
    void activate(World& w, Point p);
    void destroy(World& w);
    void clearCommandQueue(World& w);
    bool consumeEnergy(World& w, Quantity q);

    bool isStructure(const World& w) const;
    bool isWorker(const World& w) const;
    bool isArmy(const World& w) const;

    bool active() const { return state == State::Active; }
    bool activeOrBuilding() const { return state == State::Active || state == State::Building; }

    void trigger(World& w, const UnitCommand& uc, CancelOthers cancelOthers = CancelOthers::Yes);
    void trigger(
        AbilityInstanceIndex abilityIndex,
        World& w,
        const AbilityTarget& target,
        UnitId prototype = {},
        CancelOthers cancelOthers = CancelOthers::Yes);
    void trigger(
        AbilityInstanceIndex abilityIndex,
        World& w,
        TriggerGroup& group,
        const AbilityTarget& target,
        UnitId prototype = {},
        CancelOthers cancelOthers = CancelOthers::Yes);
    static WorldActionList step(UnitStableRef u, const World& w)
        __attribute__((warn_unused_result));
    void abilityStepAction(World& w, AbilityStateIndex as, const AbilityStepAction& f);
    void cancelAll(World& w);

    bool isActive(const World& w, abilities::Kind kind) const;
    static const AbilityState& abilityState(UnitStableRef u, const World& w, abilities::Kind kind);
    static GameTime nextStepTime(UnitStableRef u) { return u->nextStepTime_; }

    template<typename T>
    static std::optional<T> abilityState(UnitStableRef u, const World& w) {
      const AbilityState& as{abilityState(u, w, abilities::kind<T>())};
      return as.state<T>();
    }

    AbilityReadyState abilityReadyState(
        const World& w, AbilityInstanceIndex abilityIndex, AbilityId abilityId) const;

  private:
    void doActivate(World& w);

    mutable std::array<AbilityState, MaxUnitAbilityStates> abilityStates_;
    mutable GameTime nextStepTime_{GameTimeInf};
  };
}
