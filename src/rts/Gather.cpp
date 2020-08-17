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

void rts::abilities::state::Gather::trigger(
    World& w, Entity& e, ActiveAbilityStateUPtr& as, const Desc& desc, Point target) {
  if (as)
    as->cancel(w);
  as = std::make_unique<Gather>(desc, target);
}

rts::AbilityStepResult rts::abilities::state::Gather::step(const World& w, const Entity& e) {
  switch (state_) {
    case State::Init:
      return init(w, e);

    case State::MovingToTarget:
      if (moveAbilityState_->active())
        return MonitorTime;
      if (auto rf{w[targetField_]}) {
        if (adjacent(e.area, rf->area))
          return tryOccupy();
      }
      state_ = State::Occupying;
      return MonitorTime;

    case State::Occupying:
      bool findBlockedOk;
      if (auto* rf{w[targetField_]}) {
        if (!rf->sem.blocked())
          return tryOccupy();
        findBlockedOk = false;
      }
      else {
        findBlockedOk = true;
      }
      if (auto* rf{w.closestResourceField(e.area.topLeft, targetGroup_, findBlockedOk)}) {
        targetField_ = w.weakId(*rf);
        target_ = rf->area.topLeft;
        state_ = State::MovingToTarget;
        return moveTo(target_);
      }
      return findBlockedOk ? 0 : MonitorTime;

    case State::Gathering:
      return finishGathering();

    case State::GatheringDone: {
      if (auto* b{w.closestEntity(e.area.topLeft, e.side, desc_.baseType)}) {
        base_ = w.weakId(*b);
        state_ = State::MovingToBase;
        return moveTo(b->area.topLeft);
      }
      return 0;
    }

    case State::MovingToBase:
      if (moveAbilityState_->active())
        return MonitorTime;
      if (auto b{w[base_]}) {
        if (adjacent(e.area, b->area)) {
          state_ = State::Delivering;
          return desc_.deliverTime;
        }
      }
      return 0;

    case State::Delivering:
      return finishDelivering();

    case State::DeliveringDone:
      state_ = State::MovingToTarget;
      return moveTo(target_);
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

  moveAbilityState_ = &entity.abilityState(w, abilities::Kind::Move);

  state_ = State::MovingToTarget;
  return moveTo(target_);
}

rts::AbilityStepAction rts::abilities::state::Gather::moveTo(Point p) {
  return [this, p](World& w, Entity& e) {
    e.trigger(desc_.moveAbility, w, p, Entity::CancelOthers::No);
    return MonitorTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Gather::tryOccupy() {
  return [this](World& w, Entity&) {
    targetFieldLock_ = SemaphoreLock{w, targetField_};
    if (targetFieldLock_) {
      state_ = State::Gathering;
      return desc_.gatherTime;
    }
    state_ = State::Occupying;
    return MonitorTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Gather::finishGathering() {
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

rts::AbilityStepAction rts::abilities::state::Gather::finishDelivering() {
  return [this](World& w, Entity& e) {
    e.bag.transferAllTo(w[e.side].bag(resource_));
    state_ = State::DeliveringDone;
    return 1;
  };
}
