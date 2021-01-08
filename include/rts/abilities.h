#pragma once

#include "Ability.h"
#include "constants.h"
#include "types.h"

#include <cassert>
#include <functional>
#include <memory>
#include <type_traits>

namespace rts {
  class ActiveAbilityState;
  class ActiveAbilityGroupState;
  using ActiveAbilityStateUPtr = std::unique_ptr<ActiveAbilityState>;
  using ActiveAbilityGroupStateSPtr = std::shared_ptr<ActiveAbilityGroupState>;

  struct TriggerGroup {
    uint32_t originalSize;
    ActiveAbilityGroupStateSPtr sharedState;
  };
}

namespace rts::abilities {
  namespace state {
    class Build;
    class Gather;
    class Move;
    class Produce;
    class SetRallyPoint;
  }

  enum class Kind { None, Build, Gather, Move, Produce, SetRallyPoint, MAX };
  static_assert(size_t(Kind::MAX) - 1 == MaxUnitAbilityStates);
  enum class GroupMode { One, All };
  enum class TargetType { None, Any, Ground, Resource };

  enum class BuildState { Init, MovingToTarget, Building };

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
    if constexpr (std::is_same_v<State, BuildState>)
      return Kind::Build;
    else if constexpr (std::is_same_v<State, GatherState>)
      return Kind::Gather;
    else if constexpr (std::is_same_v<State, MoveState>)
      return Kind::Move;
    else if constexpr (std::is_same_v<State, ProduceState>)
      return Kind::Produce;
  }

  int mutualCancelGroup(Kind kind);

  struct Build {
    using AbilityState = state::Build;
    static constexpr auto kind{Kind::Build};
    static constexpr auto groupMode{GroupMode::One};
    static constexpr auto targetType{TargetType::Ground};
    static constexpr auto enqueable{true};
    static constexpr auto availableWhileBuilding{false};
    static constexpr auto requiresPower{false};
    AbilityId id;
    UnitTypeId type;
  };

  struct Gather {
    using AbilityState = state::Gather;
    static constexpr auto kind{Kind::Gather};
    static constexpr auto groupMode{GroupMode::All};
    static constexpr auto targetType{TargetType::Resource};
    static constexpr auto enqueable{true};
    static constexpr auto availableWhileBuilding{false};
    static constexpr auto requiresPower{false};
    AbilityId id;
    GameTime gatherTime;
    GameTime deliverTime;
  };

  struct GoToPage {
    static constexpr auto kind{Kind::None};
    AbilityId id;
    AbilityPage page;
  };

  struct Move {
    using AbilityState = state::Move;
    static constexpr auto kind{Kind::Move};
    static constexpr auto groupMode{GroupMode::All};
    static constexpr auto targetType{TargetType::Any};
    static constexpr auto enqueable{true};
    static constexpr auto availableWhileBuilding{false};
    static constexpr auto requiresPower{false};
    AbilityId id;
    Speed speed;
  };

  struct Produce {
    using AbilityState = state::Produce;
    static constexpr auto kind{Kind::Produce};
    static constexpr auto groupMode{GroupMode::One};
    static constexpr auto targetType{TargetType::None};
    static constexpr auto enqueable{false};
    static constexpr auto availableWhileBuilding{false};
    static constexpr auto requiresPower{true};
    AbilityId id;
    UnitTypeId type;
  };

  struct SetRallyPoint {
    using AbilityState = state::SetRallyPoint;
    static constexpr auto kind{Kind::SetRallyPoint};
    static constexpr auto groupMode{GroupMode::All};
    static constexpr auto targetType{TargetType::Any};
    static constexpr auto enqueable{false};
    static constexpr auto availableWhileBuilding{true};
    static constexpr auto requiresPower{false};
    AbilityId id;
  };

  struct Instance {
    using Trigger = std::function<std::pair<ActiveAbilityStateUPtr, GameTime>(
        World&, Unit&, TriggerGroup&, ActiveAbilityStateUPtr&, const AbilityTarget&, UnitId)>;
    using AbstractDesc = std::function<const void*()>;

    Kind kind{Kind::None};
    AbilityId abilityId{};
    AbilityPage goToPage{0};
    GroupMode groupMode{};
    TargetType targetType{};
    bool enqueable{false};
    bool availableWhileBuilding{false};
    bool requiresPower{false};
    AbilityStateIndex stateIndex{AbilityStateIndex::None};
    Trigger trigger;
    AbstractDesc abstractDesc;

    template<typename D>
    const D& desc() const {
      assert(D::kind == kind);
      return *static_cast<const D*>(abstractDesc());
    }
  };

  inline bool mutualCancelling(const Instance& i1, const Instance& i2) {
    auto group{mutualCancelGroup(i1.kind)};
    return group && group == mutualCancelGroup(i2.kind);
  }

  template<typename D>
  Instance instance(const D& desc, AbilityStateIndex as);
  Instance instance(const GoToPage& desc, AbilityStateIndex);
}
