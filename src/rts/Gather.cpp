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

rts::GameTime rts::abilities::state::Gather::step(
    const World& w,
    const Entity& entity,
    EntityAbilityIndex abilityIndex,
    WorldActionList& actions) {
  switch (state_) {
    case State::Init:
      return init(w, entity, actions);

    case State::MovingToTarget:
      if (moveAbilityState_->active())
        return MonitorTime;
      if (auto rf{w[targetField_]}) {
        if (adjacent(entity.area, rf->area)) {
          state_ = State::Occupying;
          addStepAction(w, entity, abilityIndex, actions);
          return GameTimeInf;
        }
      }
      state_ = State::Occupying;
      return MonitorTime;

    case State::Occupying:
      bool findBlockedOk;
      if (auto* rf{w[targetField_]}) {
        if (!rf->sem.blocked()) {
          addStepAction(w, entity, abilityIndex, actions);
          return GameTimeInf;
        }
        findBlockedOk = false;
      }
      else {
        findBlockedOk = true;
      }
      if (auto* rf{w.closestResourceField(entity.area.topLeft, targetGroup_, findBlockedOk)}) {
        targetField_ = w.weakId(*rf);
        target_ = rf->area.topLeft;
        return moveTo(target_, State::MovingToTarget, w, entity, actions);
      }
      return findBlockedOk ? 0 : MonitorTime;

    case State::Gathering:
      addStepAction(w, entity, abilityIndex, actions);
      return GameTimeInf;

    case State::GatheringDone: {
      if (auto* b{w.closestEntity(entity.area.topLeft, entity.side, baseType_)}) {
        base_ = w.weakId(*b);
        return moveTo(b->area.topLeft, State::MovingToBase, w, entity, actions);
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
      addStepAction(w, entity, abilityIndex, actions);
      return GameTimeInf;

    case State::DeliveringDone:
      return moveTo(target_, State::MovingToTarget, w, entity, actions);
  }
  return 0;
}

rts::GameTime rts::abilities::state::Gather::stepAction(World& w, Entity& entity) {
  switch (state_) {
    case State::Occupying:
      targetFieldLock_ = SemaphoreLock{w, targetField_};
      if (targetFieldLock_) {
        state_ = State::Gathering;
        return gatherTime_;
      }
      return MonitorTime;

    case State::Gathering:
      if (auto rf{w[targetField_]}) {
        rf->bag.transferAllTo(entity.bag);
        targetFieldLock_.release(w);
        if (rf->bag.empty())
          w.destroy(*rf);
        state_ = State::GatheringDone;
      }
      else {
        state_ = State::Occupying;
      }
      return 1;

    case State::Delivering:
      entity.bag.transferAllTo(w[entity.side].bag(resource_));
      state_ = State::DeliveringDone;
      return 1;

    default:
      return 0;
  }
}

void rts::abilities::state::Gather::cancel(const World& w, WorldActionList& actions) {
  if (targetFieldLock_)
    addReleaseAction(actions, targetFieldLock_.detach());
}

rts::GameTime rts::abilities::state::Gather::init(
    const World& w, const Entity& entity, WorldActionList& actions) {
  auto* rf{w.resourceField(target_)};
  if (!rf)
    return 0;
  targetField_ = w.weakId(*rf);
  resource_ = rf->bag.resource();
  targetGroup_ = rf->group;

  auto ai{w[entity.type].abilityIndex(moveAbility_)};
  assert(ai != EntityAbilityIndex::None);
  moveAbilityState_ = &entity.abilityStates[ai];

  return moveTo(target_, State::MovingToTarget, w, entity, actions);
}

rts::GameTime rts::abilities::state::Gather::moveTo(
    Point p, State s, const World& w, const Entity& entity, WorldActionList& actions) {
  state_ = s;
  entity.triggerNested(moveAbility_, w, p, actions);
  return MonitorTime;
}
