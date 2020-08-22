#pragma once

#include "Ability.h"
#include "types.h"

#include <functional>

namespace rts::abilities {
  enum class GatherState {
    Init,
    MovingToTarget,
    Occupying,
    Gathering,
    GatheringDone,
    MovingToBase,
    Delivering,
    DeliveringDone
  };

  enum class Kind { None, Gather, Move, Produce };

  enum class MoveState { Moving };
  enum class ProduceState { Idle, Producing };

  struct Gather {
    static constexpr auto kind{Kind::Gather};
    AbilityId id;
    AbilityId moveAbility;
    UnitTypeId baseType;
    GameTime gatherTime;
    GameTime deliverTime;
  };

  struct Move {
    static constexpr auto kind{Kind::Move};
    AbilityId id;
    Speed speed;
  };

  struct Produce {
    static constexpr auto kind{Kind::Produce};
    AbilityId id;
    UnitTypeId type;
  };

  struct Instance {
    using Trigger = std::function<void(World&, Unit&, ActiveAbilityStateUPtr&, Point)>;

    Kind kind{Kind::None};
    AbilityId abilityId;
    AbilityStateIndex stateIndex{AbilityStateIndex::None};
    Trigger trigger;
  };

  Instance instance(const Gather& desc, AbilityStateIndex as);
  Instance instance(const Move& desc, AbilityStateIndex as);
  Instance instance(const Produce& desc, AbilityStateIndex as);
}
