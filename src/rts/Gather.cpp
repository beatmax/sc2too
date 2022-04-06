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

rts::ActiveAbilityStateUPtr rts::abilities::state::Gather::trigger(
    World& w, Unit& u, ActiveAbilityStateUPtr& as, const Desc& desc, const AbilityTarget& target) {
  auto tp{w.center(target)};
  if (!w.resourceFieldId(tp, u.side))
    return {};
  return std::make_unique<Gather>(desc, w.abilityWeakTarget(target), tp);
}

rts::AbilityStepResult rts::abilities::state::Gather::step(const World& w, UnitStableRef u) {
  switch (state_) {
    case State::Init:
      return init(w, u);

    case State::MovingToTarget:
      if (Unit::abilityState(u, w, Kind::Move).active())
        return MonitorTime;
      if (auto* rf{w.resourceField(target_, u->side)}; rf && !rf->bag.empty()) {
        if (adjacent(u->area, rf->area))
          return tryOccupy(w.weakId(*rf));
      }
      state_ = State::Occupying;
      return MonitorTime;

    case State::Occupying: {
      bool findBlockedOk;
      if (auto* rf{w.resourceField(target_, u->side)}; rf && !rf->bag.empty()) {
        if (adjacent(u->area, rf->area)) {
          if (!rf->sem.blocked())
            return tryOccupy(w.weakId(*rf));
        }
        findBlockedOk = false;
      }
      else {
        findBlockedOk = true;
      }
      if (targetGroup_) {
        if (auto* rf{w.closestResourceField(u->area.topLeft, targetGroup_, findBlockedOk)}) {
          targetPoint_ = rf->area.center();
          target_ = w.abilityWeakTarget(targetPoint_);
          state_ = State::MovingToTarget;
          return moveTo(targetPoint_);
        }
      }
      return findBlockedOk ? 0 : MonitorTime;
    }

    case State::Gathering:
      return finishGathering();

    case State::GatheringDone:
      return moveToBase(w, u);

    case State::MovingToBase:
      if (Unit::abilityState(u, w, Kind::Move).active())
        return MonitorTime;
      if (auto b{w[base_]}) {
        if (adjacent(u->area, b->area)) {
          state_ = State::Delivering;
          return desc_.deliverTime;
        }
      }
      return moveToBase(w, u);

    case State::Delivering:
      return finishDelivering();

    case State::DeliveringDone:
      if (u->commandQueue.empty()) {
        state_ = State::MovingToTarget;
        return moveTo(targetPoint_);
      }
      return GameTime{0};
  }
  return GameTime{0};
}

void rts::abilities::state::Gather::cleanup(World& w) {
  targetFieldLock_.release(w);
  workerCountRef_.release(w);
}

rts::AbilityStepResult rts::abilities::state::Gather::init(const World& w, const Unit& unit) {
  auto* rf{w.resourceField(target_, unit.side)};
  if (!rf)
    return GameTime{0};
  targetGroup_ = rf->group;

  if (auto* b{w.closestActiveUnit(rf->area.center(), unit.side, w[unit.side].baseType())})
    base_ = w.weakId(*b);

  state_ = State::MovingToTarget;
  return moveTo(targetPoint_);
}

rts::AbilityStepResult rts::abilities::state::Gather::moveToBase(const World& w, const Unit& unit) {
  if (auto* b{w.closestActiveUnit(unit.area.topLeft, unit.side, w[unit.side].baseType())}) {
    base_ = w.weakId(*b);
    state_ = State::MovingToBase;
    return moveTo(b->area.topLeft);
  }
  return GameTime{0};
}

rts::AbilityStepAction rts::abilities::state::Gather::moveTo(Point p) {
  return [this, p](World& w, Unit& u) {
    updateWorkerCountRef(w);
    auto moveIndex{w[u.type].abilityIndex(Kind::Move)};
    assert(moveIndex != AbilityStateIndex::None);
    u.trigger(moveIndex, w, p, {}, Unit::CancelOthers::No);
    return MonitorTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Gather::tryOccupy(ResourceFieldWId rf) {
  return [this, rf](World& w, Unit&) {
    targetFieldLock_ = SemaphoreLock{w, rf};
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
    if (auto* rf{w.resourceField(target_, u.side)}) {
      rf->bag.transferAllTo(u.bag);
      targetFieldLock_.release(w);
      if (rf->bag.empty() && rf->destroyWhenEmpty == ResourceField::DestroyWhenEmpty::Yes)
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
    u.bag.transferAllTo(w[u.side].resources());
    state_ = State::DeliveringDone;
    return 1;
  };
}

void rts::abilities::state::Gather::updateWorkerCountRef(World& w) {
  workerCountRef_.release(w);
  if (auto* rfBuildingWId{std::get_if<UnitWId>(&target_)})
    workerCountRef_ = WorkerCountRef{w, *rfBuildingWId};
  else
    workerCountRef_ = WorkerCountRef{w, base_};
}
