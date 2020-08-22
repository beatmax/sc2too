#include "Gather.h"

#include "rts/Unit.h"
#include "rts/World.h"
#include "rts/constants.h"

#include <cassert>

namespace rts {
  namespace {
    constexpr GameTime MonitorTime{10};
  }
}

void rts::abilities::state::Gather::trigger(
    World& w, Unit& u, ActiveAbilityStateUPtr& as, const Desc& desc, Point target) {
  if (as)
    as->cancel(w);
  as = std::make_unique<Gather>(desc, target);
}

rts::AbilityStepResult rts::abilities::state::Gather::step(const World& w, UnitStableRef u) {
  switch (state_) {
    case State::Init:
      return init(w, u);

    case State::MovingToTarget:
      if (Unit::abilityState(u, w, abilities::Kind::Move).active())
        return MonitorTime;
      if (auto rf{w[targetField_]}) {
        if (adjacent(u->area, rf->area))
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
      if (auto* rf{w.closestResourceField(u->area.topLeft, targetGroup_, findBlockedOk)}) {
        targetField_ = w.weakId(*rf);
        target_ = rf->area.topLeft;
        state_ = State::MovingToTarget;
        return moveTo(target_);
      }
      return findBlockedOk ? 0 : MonitorTime;

    case State::Gathering:
      return finishGathering();

    case State::GatheringDone: {
      if (auto* b{w.closestUnit(u->area.topLeft, u->side, desc_.baseType)}) {
        base_ = w.weakId(*b);
        state_ = State::MovingToBase;
        return moveTo(b->area.topLeft);
      }
      return GameTime{0};
    }

    case State::MovingToBase:
      if (Unit::abilityState(u, w, abilities::Kind::Move).active())
        return MonitorTime;
      if (auto b{w[base_]}) {
        if (adjacent(u->area, b->area)) {
          state_ = State::Delivering;
          return desc_.deliverTime;
        }
      }
      return GameTime{0};

    case State::Delivering:
      return finishDelivering();

    case State::DeliveringDone:
      state_ = State::MovingToTarget;
      return moveTo(target_);
  }
  return GameTime{0};
}

void rts::abilities::state::Gather::cancel(World& w) {
  if (targetFieldLock_)
    targetFieldLock_.release(w);
}

rts::AbilityStepResult rts::abilities::state::Gather::init(const World& w, const Unit& unit) {
  auto* rf{w.resourceField(target_)};
  if (!rf)
    return GameTime{0};
  targetField_ = w.weakId(*rf);
  resource_ = rf->bag.resource();
  targetGroup_ = rf->group;

  state_ = State::MovingToTarget;
  return moveTo(target_);
}

rts::AbilityStepAction rts::abilities::state::Gather::moveTo(Point p) {
  return [this, p](World& w, Unit& u) {
    u.trigger(desc_.moveAbility, w, p, Unit::CancelOthers::No);
    return MonitorTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Gather::tryOccupy() {
  return [this](World& w, Unit&) {
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
  return [this](World& w, Unit& u) {
    if (auto rf{w[targetField_]}) {
      rf->bag.transferAllTo(u.bag);
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
  return [this](World& w, Unit& u) {
    u.bag.transferAllTo(w[u.side].bag(resource_));
    state_ = State::DeliveringDone;
    return 1;
  };
}
