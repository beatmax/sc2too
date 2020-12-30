#pragma once

#include "rts/AbilityState.h"
#include "rts/Path.h"
#include "rts/abilities.h"
#include "rts/types.h"

#include <optional>

namespace rts::abilities::state {
  class Move : public ActiveAbilityGroupStateTpl<Move> {
  public:
    using Desc = abilities::Move;
    using State = abilities::MoveState;

    struct GroupState : ActiveAbilityGroupState {
      float closeEnough;
      size_t count{0};
    };
    using GroupStateSPtr = std::shared_ptr<GroupState>;

    static std::pair<ActiveAbilityStateUPtr, GameTime> trigger(
        World& w,
        Unit& u,
        TriggerGroup& group,
        ActiveAbilityStateUPtr& as,
        const Desc& desc,
        const AbilityTarget& target);

    explicit Move(const Desc& desc, AbilityTarget target, GroupStateSPtr gs)
      : desc_{desc}, target_{target}, groupState_{gs} {}
    AbilityStepResult step(const World& w, UnitStableRef u);
    void cancel(World& w) final {}
    int state() const final { return int(State::Moving); }

  private:
    AbilityStepAction stepAction();

    const Desc desc_;
    AbilityTarget target_;
    GroupStateSPtr groupState_;
    ExcludedPointSet excludedPoints_;
    Path path_;
    bool havePath_{false};
    std::optional<std::pair<Point, UnitId>> waitingFor_;
    int waitingTries{0};
  };
}
