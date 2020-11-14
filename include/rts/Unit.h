#pragma once

#include "AbilityState.h"
#include "Resource.h"
#include "WorldObject.h"
#include "constants.h"
#include "types.h"
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
    ResourceFlexBag bag;
    ProductionQueueId productionQueue;
    std::array<bool, MaxUnitAbilityStates> abilityActive{};

    Unit(
        Vector s,
        UnitTypeId t,
        SideId sd,
        UiUPtr ui,
        Quantity cargoCapacity = 0,
        ProductionQueueId pq = {});
    void allocate(World& w, bool allocCheck = true);
    bool tryAllocate(World& w);
    void setBuildPoint(World& w, Point p);
    bool tryStartBuilding(World& w);
    void finishBuilding(World& w);
    void activate(World& w, Point p);
    void destroy(World& w);

    bool isStructure(const World& w) const;
    bool isWorker(const World& w) const;
    bool isArmy(const World& w) const;

    bool active() const { return state == State::Active; }
    bool activeOrBuilding() const { return state == State::Active || state == State::Building; }
    bool hasEnabledAbility(
        const World& w, AbilityInstanceIndex abilityIndex, AbilityId abilityId) const;

    void trigger(
        AbilityInstanceIndex abilityIndex,
        World& w,
        const AbilityTarget& target,
        CancelOthers cancelOthers = CancelOthers::Yes);
    void trigger(
        AbilityInstanceIndex abilityIndex,
        World& w,
        TriggerGroup& group,
        const AbilityTarget& target,
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

  private:
    void doActivate(World& w);

    mutable std::array<AbilityState, MaxUnitAbilityStates> abilityStates_;
    mutable GameTime nextStepTime_{GameTimeInf};
  };
}
