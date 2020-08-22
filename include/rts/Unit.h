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
    enum class CancelOthers { No, Yes };

    UnitTypeId type;
    SideId side;
    ResourceBag bag;
    ProductionQueueId productionQueue;

    Unit(
        Point p,
        Vector s,
        UnitTypeId t,
        SideId sd,
        UiUPtr ui,
        Quantity cargoCapacity = 0,
        ProductionQueueId pq = {});
    void onDestroy(World& w);

    void trigger(
        AbilityId ability, World& w, Point target, CancelOthers cancelOthers = CancelOthers::Yes);
    static WorldActionList step(UnitStableRef u, const World& w)
        __attribute__((warn_unused_result));
    void abilityStepAction(World& w, AbilityStateIndex as, const AbilityStepAction& f);
    void cancelAll(World& w);

    static const AbilityState& abilityState(UnitStableRef u, const World& w, abilities::Kind kind);
    static GameTime nextStepTime(UnitStableRef u) { return u->nextStepTime_; }

    template<typename T>
    static std::optional<T> state(UnitStableRef u, const World& w) {
      const AbilityState& as{abilityState(u, w, abilities::kind<T>())};
      return as.state<T>();
    }

  private:
    mutable std::array<AbilityState, MaxUnitAbilities> abilityStates_;
    mutable GameTime nextStepTime_{GameTimeInf};

    const AbilityStateIndex activeAbilityStateIndex(abilities::Kind kind) const;
  };
}
