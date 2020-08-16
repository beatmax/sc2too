#include "Gather.h"

#include "rts/Entity.h"
#include "rts/World.h"
#include "rts/constants.h"

#include <cassert>

namespace rts {
  namespace {
    constexpr GameTime MonitorTime{10};
  }
}

rts::AbilityStepResult rts::abilities::state::Gather::step(
    const World& w, const Entity& entity, EntityAbilityIndex abilityIndex) {
  switch (state_) {
    case State::Init:
      return init(w, entity);

    case State::MovingToTarget:
      if (moveAbilityState_->active())
        return MonitorTime;
      if (auto rf{w[targetField_]}) {
        if (adjacent(entity.area, rf->area))
          return tryOccupy(w);
      }
      state_ = State::Occupying;
      return MonitorTime;

    case State::Occupying:
      bool findBlockedOk;
      if (auto* rf{w[targetField_]}) {
        if (!rf->sem.blocked())
          return tryOccupy(w);
        findBlockedOk = false;
      }
      else {
        findBlockedOk = true;
      }
      if (auto* rf{w.closestResourceField(entity.area.topLeft, targetGroup_, findBlockedOk)}) {
        targetField_ = w.weakId(*rf);
        target_ = rf->area.topLeft;
        state_ = State::MovingToTarget;
        return moveTo(target_, w, entity);
      }
      return findBlockedOk ? 0 : MonitorTime;

    case State::Gathering:
      return finishGathering(w);

    case State::GatheringDone: {
      if (auto* b{w.closestEntity(entity.area.topLeft, entity.side, baseType_)}) {
        base_ = w.weakId(*b);
        state_ = State::MovingToBase;
        return moveTo(b->area.topLeft, w, entity);
      }
      return 0;
    }

    case State::MovingToBase:
      if (moveAbilityState_->active())
        return MonitorTime;
      if (auto b{w[base_]}) {
        if (adjacent(entity.area, b->area)) {
          state_ = State::Delivering;
          return deliverTime_;
        }
      }
      return 0;

    case State::Delivering:
      return finishDelivering(w);

    case State::DeliveringDone:
      state_ = State::MovingToTarget;
      return moveTo(target_, w, entity);
  }
  return 0;
}

void rts::abilities::state::Gather::cancel(World& w) {
  if (targetFieldLock_)
    targetFieldLock_.release(w);
}

rts::AbilityStepResult rts::abilities::state::Gather::init(const World& w, const Entity& entity) {
  auto* rf{w.resourceField(target_)};
  if (!rf)
    return 0;
  targetField_ = w.weakId(*rf);
  resource_ = rf->bag.resource();
  targetGroup_ = rf->group;

  auto ai{w[entity.type].abilityIndex(moveAbility_)};
  assert(ai != EntityAbilityIndex::None);
  moveAbilityState_ = &entity.abilityStates[ai];

  state_ = State::MovingToTarget;
  return moveTo(target_, w, entity);
}

rts::AbilityStepAction rts::abilities::state::Gather::moveTo(
    Point p, const World& w, const Entity& entity) {
  return [this, p](World& w, Entity& e) {
    e.trigger(moveAbility_, w, p, Entity::CancelOthers::No);
    return MonitorTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Gather::tryOccupy(const World& w) {
  return [this](World& w, Entity&) {
    targetFieldLock_ = SemaphoreLock{w, targetField_};
    if (targetFieldLock_) {
      state_ = State::Gathering;
      return gatherTime_;
    }
    state_ = State::Occupying;
    return MonitorTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Gather::finishGathering(const World& w) {
  return [this](World& w, Entity& e) {
    if (auto rf{w[targetField_]}) {
      rf->bag.transferAllTo(e.bag);
      targetFieldLock_.release(w);
      if (rf->bag.empty())
        w.destroy(*rf);
      state_ = State::GatheringDone;
    }
    else {
      state_ = State::Occupying;
    }
    return 1;
  };
}

rts::AbilityStepAction rts::abilities::state::Gather::finishDelivering(const World& w) {
  return [this](World& w, Entity& e) {
    e.bag.transferAllTo(w[e.side].bag(resource_));
    state_ = State::DeliveringDone;
    return 1;
  };
}
