#pragma once

#include "Ability.h"
#include "types.h"

#include <functional>
#include <type_traits>

namespace rts::abilities {
  enum class Kind { None, Gather, Move, Produce, SetRallyPoint };

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

  enum class MoveState { Moving };
  enum class ProduceState { Idle, Blocked, Producing };

  template<typename State>
  inline constexpr Kind kind() {
    if constexpr (std::is_same_v<State, GatherState>)
      return Kind::Gather;
    else if constexpr (std::is_same_v<State, MoveState>)
      return Kind::Move;
    else if constexpr (std::is_same_v<State, ProduceState>)
      return Kind::Produce;
  }

  int mutualCancelGroup(Kind kind);

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

  struct SetRallyPoint {
    static constexpr auto kind{Kind::SetRallyPoint};
    AbilityId id;
  };

  struct Instance {
    using Trigger = std::function<void(World&, Unit&, ActiveAbilityStateUPtr&, Point)>;

    Kind kind{Kind::None};
    AbilityId abilityId;
    AbilityStateIndex stateIndex{AbilityStateIndex::None};
    Trigger trigger;
  };

  inline bool mutualCancelling(const Instance& i1, const Instance& i2) {
    auto group{mutualCancelGroup(i1.kind)};
    return group && group == mutualCancelGroup(i2.kind);
  }

  Instance instance(const Gather& desc, AbilityStateIndex as);
  Instance instance(const Move& desc, AbilityStateIndex as);
  Instance instance(const Produce& desc, AbilityStateIndex as);
  Instance instance(const SetRallyPoint& desc, AbilityStateIndex as);
}
