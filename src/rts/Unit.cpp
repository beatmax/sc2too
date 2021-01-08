#include "rts/Unit.h"

#include "rts/World.h"
#include "rts/WorldAction.h"

#include <algorithm>
#include <cassert>

rts::Unit::Unit(
    Vector s,
    UnitTypeId t,
    SideId sd,
    UiUPtr ui,
    Quantity cargoCapacity,
    ProductionQueueId pq,
    ResourceFieldId rf)
  : WorldObject{{-1, -1}, s, std::move(ui)},
    type{t},
    side{sd},
    bag{{}, 0, cargoCapacity},
    productionQueue{pq},
    resourceField{rf} {
}

void rts::Unit::allocate(World& w, bool allocCheck) {
  const auto& t{w[type]};
  auto& res{w[side].resources()};
  AllocResult ar{
      res.allocate(t.cost, AllocFilter::Any, allocCheck ? AllocMode::Check : AllocMode::NoCheck)};
  if (ar.result != AllocResult::Success)
    throw std::runtime_error{"adding unit: resource allocation failed (use addForFree()?)"};
  state = State::Allocated;
}

bool rts::Unit::tryAllocate(World& w) {
  const auto& t{w[type]};
  auto& s{w[side]};
  auto& res{s.resources()};
  if (auto [result, failedResource] = res.allocate(t.cost); result == AllocResult::Success) {
    state = State::Allocated;
    return true;
  }
  else {
    auto& ui{w[failedResource].ui()};
    s.messages().add(w, result == AllocResult::Lack ? ui.msgMoreRequired() : ui.msgCapReached());
    return false;
  }
}

void rts::Unit::setBuildPoint(World& w, Point p) {
  assert(state == State::Allocated);
  area.topLeft = p;
  layer = LayerPrototypes;
  w[side].prototypeMap().setContent(area, w.id(*this));
  state = State::Buildable;
}

bool rts::Unit::tryStartBuilding(World& w) {
  assert(state == State::Buildable);
  if (w.tryPlace(*this)) {
    state = State::Building;
    w[side].prototypeMap().setContent(area, Cell::Empty{});
    nextStepTime_ = w.time + w[type].buildTime;
    return true;
  }
  return false;
}

void rts::Unit::finishBuilding(World& w) {
  assert(state == State::Building);
  doActivate(w);
}

void rts::Unit::activate(World& w, Point p) {
  assert(state == State::Allocated);
  area.topLeft = p;
  w.place(*this);
  doActivate(w);
}

void rts::Unit::destroy(World& w) {
  assert(state != State::Destroyed);

  cancelAll(w);
  clearCommandQueue(w);
  if (productionQueue)
    w.destroy(productionQueue);

  auto& s{w[side]};
  if (state != State::New) {
    const auto& t{w[type]};
    auto& res{s.resources()};
    if (state == State::Building || state == State::Active) {
      res.deallocate(t.cost);
      w.remove(*this);
      if (state == State::Active) {
        res.deprovision(t.provision);
        if (auto er{t.powerRadius})
          s.powerMap().update(w, side, circleCenteredAt(area, er), -1);
      }
    }
    else {
      if (state == State::Buildable)
        s.prototypeMap().setContent(area, Cell::Empty{});
      res.restore(t.cost);
    }
  }

  state = State::Destroyed;
}

void rts::Unit::clearCommandQueue(World& w) {
  for (size_t i{0}; i < commandQueue.size(); ++i) {
    const rts::UnitCommand& cmd{commandQueue[i]};
    if (cmd.prototype)
      w.destroy(cmd.prototype);
  }
  commandQueue.clear();
}

bool rts::Unit::isStructure(const World& w) const {
  return w[type].kind == UnitType::Kind::Structure;
}

bool rts::Unit::isWorker(const World& w) const {
  return w[type].kind == UnitType::Kind::Worker;
}

bool rts::Unit::isArmy(const World& w) const {
  return w[type].kind == UnitType::Kind::Army;
}

bool rts::Unit::hasEnabledAbility(
    const World& w, AbilityInstanceIndex abilityIndex, AbilityId abilityId) const {
  const auto& abilityInstance{w[type].abilities[abilityIndex]};
  return abilityInstance.abilityId == abilityId && (powered || !abilityInstance.requiresPower) &&
      (state == State::Active ||
       (state == State::Building && abilityInstance.availableWhileBuilding));
}

void rts::Unit::trigger(World& w, const UnitCommand& uc, CancelOthers cancelOthers) {
  if (auto target{w.fromWeakTarget(uc.target)}) {
    auto& group{w.triggerGroups[uc.triggerGroupId]};
    group.originalSize = uc.triggerGroupSize;
    trigger(uc.abilityIndex, w, group, *target, uc.prototype, cancelOthers);
  }
}

void rts::Unit::trigger(
    AbilityInstanceIndex abilityIndex,
    World& w,
    const AbilityTarget& target,
    UnitId prototype,
    CancelOthers cancelOthers) {
  TriggerGroup group{1};
  trigger(abilityIndex, w, group, target, prototype, cancelOthers);
}

void rts::Unit::trigger(
    AbilityInstanceIndex abilityIndex,
    World& w,
    TriggerGroup& group,
    const AbilityTarget& target,
    UnitId prototype,
    CancelOthers cancelOthers) {
  const auto& unitType{w[type]};
  const auto& abilityInstance{unitType.abilities[abilityIndex]};
  if (abilityInstance.kind == abilities::Kind::None)
    return;

  if (!(state == State::Active ||
        (state == State::Building && abilityInstance.availableWhileBuilding)))
    return;

  AbilityState& abilityState{abilityStates_[abilityInstance.stateIndex]};

  if (cancelOthers == CancelOthers::Yes) {
    for (const auto& otherInstance : unitType.abilities) {
      auto& as{abilityStates_[otherInstance.stateIndex]};
      if (&as != &abilityState && as.active() && mutualCancelling(abilityInstance, otherInstance))
        as.cancel(w, *this, otherInstance.stateIndex);
    }
  }

  abilityState.trigger(w, *this, group, abilityInstance, target, prototype);
  nextStepTime_ = std::min(nextStepTime_, abilityState.nextStepTime());
}

rts::WorldActionList rts::Unit::step(UnitStableRef u, const World& w) {
  WorldActionList actions;
  if (u->nextStepTime_ == GameTimeInf && !u->commandQueue.empty() &&
      (w.time % u->commandQueue.front().triggerGroupSize) == 0) {  // give some time for regrouping
    actions += [wid{w.weakId(*u)}](World& w) {
      if (auto* u{w[wid]}) {
        auto cmd{u->commandQueue.front()};
        u->commandQueue.pop();
        u->trigger(w, cmd);
      }
    };
    return actions;
  }

  if (u->nextStepTime_ != w.time) {
    assert(u->nextStepTime_ > w.time);
    return actions;
  }

  if (u->state == State::Building) {
    actions += [wid{w.weakId(*u)}](World& w) {
      if (auto* u{w[wid]})
        u->finishBuilding(w);
    };
    u->nextStepTime_ = GameTimeInf;
    return actions;
  }

  u->nextStepTime_ = GameTimeInf;
  for (size_t i = 0; i < MaxUnitAbilityStates; ++i) {
    AbilityStateIndex as{i};
    auto& abilityState{u->abilityStates_[as]};
    if (abilityState.nextStepTime() == w.time)
      abilityState.step(w, u, as, actions);
    u->nextStepTime_ = std::min(u->nextStepTime_, abilityState.nextStepTime());
  }
  return actions;
}

void rts::Unit::abilityStepAction(World& w, AbilityStateIndex as, const AbilityStepAction& f) {
  auto& abilityState{abilityStates_[as]};
  abilityState.stepAction(w, *this, as, f);
  nextStepTime_ = std::min(nextStepTime_, abilityState.nextStepTime());
}

void rts::Unit::cancelAll(World& w) {
  for (size_t i = 0; i < MaxUnitAbilityStates; ++i) {
    AbilityStateIndex as{i};
    auto& abilityState{abilityStates_[as]};
    abilityState.cancel(w, *this, as);
  }
}

bool rts::Unit::isActive(const World& w, abilities::Kind kind) const {
  auto as{w[type].abilityStateIndex(kind)};
  return as != AbilityStateIndex::None && abilityActive[as];
}

const rts::AbilityState& rts::Unit::abilityState(
    UnitStableRef u, const World& w, abilities::Kind kind) {
  auto& t{w[u->type]};
  auto as{t.abilityStateIndex(kind)};
  assert(as != AbilityStateIndex::None);
  return u->abilityStates_[as];
}

void rts::Unit::doActivate(World& w) {
  auto& s{w[side]};
  const auto& t{w[type]};
  auto& res{s.resources()};
  res.provision(t.provision);
  if (auto er{t.powerRadius})
    s.powerMap().update(w, side, circleCenteredAt(area, er), 1);
  if (t.requiresPower == UnitType::RequiresPower::Yes)
    powered = s.powerMap().isActive(area.center());
  state = State::Active;
}
