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

    mutable std::array<AbilityState, MaxUnitAbilities> abilityStates;
    mutable GameTime nextStepTime{GameTimeInf};

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
    WorldActionList step(const World& w) const __attribute__((warn_unused_result));
    void abilityStepAction(World& w, AbilityStateIndex as, const AbilityStepAction& f);
    void cancelAll(World& w);

    template<typename T>
    std::optional<T> state(abilities::Kind kind) const {
      if (auto as{activeAbilityStateIndex(kind)}; as != AbilityStateIndex::None)
        return abilityStates[as].state<T>();
      return std::nullopt;
    }

    const AbilityState& abilityState(const World& w, abilities::Kind kind) const;

  private:
    const AbilityStateIndex activeAbilityStateIndex(abilities::Kind kind) const;
  };
}
