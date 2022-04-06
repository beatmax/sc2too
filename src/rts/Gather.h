#pragma once

#include "rts/AbilityState.h"
#include "rts/CountedReference.h"
#include "rts/SemaphoreLock.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class Gather : public ActiveAbilityStateTpl<Gather> {
  public:
    using Desc = abilities::Gather;
    using State = abilities::GatherState;

    static ActiveAbilityStateUPtr trigger(
        World& w,
        Unit& u,
        ActiveAbilityStateUPtr& as,
        const Desc& desc,
        const AbilityTarget& target);

    explicit Gather(const Desc& desc, AbilityWeakTarget target, Point tp)
      : desc_{desc}, target_{target}, targetPoint_{tp} {}

    AbilityStepResult step(const World& w, UnitStableRef u) final;
    void cleanup(World& w) final;
    int state() const final { return int(state_); }

  private:
    using WorkerCountRef = CountedReference<Unit, &Unit::workerCount>;

    AbilityStepResult init(const World& w, const Unit& unit);
    AbilityStepResult moveToBase(const World& w, const Unit& unit);
    AbilityStepAction moveTo(Point p);
    AbilityStepAction tryOccupy(ResourceFieldWId rf);
    AbilityStepAction finishGathering();
    AbilityStepAction finishDelivering();
    void updateWorkerCountRef(World& w);

    const Desc desc_;
    State state_{State::Init};
    AbilityWeakTarget target_;
    Point targetPoint_;
    SemaphoreLock<ResourceField> targetFieldLock_;
    ResourceGroupId targetGroup_{};
    UnitWId base_;
    WorkerCountRef workerCountRef_;
  };
}
