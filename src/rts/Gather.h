#pragma once

#include "rts/AbilityState.h"
#include "rts/SemaphoreLock.h"
#include "rts/abilities.h"
#include "rts/types.h"

namespace rts::abilities::state {
  class Gather : public ActiveAbilityStateTpl<Gather> {
  public:
    explicit Gather(
        Point target,
        AbilityId moveAbility,
        EntityTypeId baseType,
        GameTime gatherTime,
        GameTime deliverTime)
      : target_{target},
        moveAbility_{moveAbility},
        baseType_{baseType},
        gatherTime_{gatherTime},
        deliverTime_{deliverTime} {}
    AbilityStepResult step(
        const World& w, const Entity& entity, EntityAbilityIndex abilityIndex) final;
    void cancel(World& w) final;
    int state() const final { return int(state_); }

  private:
    using State = abilities::GatherState;

    AbilityStepResult init(const World& w, const Entity& entity);
    AbilityStepAction moveTo(Point p, const World& w, const Entity& entity);
    AbilityStepAction tryOccupy(const World& w);
    AbilityStepAction finishGathering(const World& w);
    AbilityStepAction finishDelivering(const World& w);

    State state_{State::Init};
    Point target_;
    const AbilityId moveAbility_;
    const EntityTypeId baseType_;
    const GameTime gatherTime_;
    const GameTime deliverTime_;
    const AbilityState* moveAbilityState_{};
    EntityWId base_;
    ResourceFieldWId targetField_;
    SemaphoreLock<ResourceField> targetFieldLock_;
    ResourceCPtr resource_{};
    ResourceGroupId targetGroup_{};
  };
}
