#include "rts/ProductionQueue.h"

#include "rts/World.h"

#include <algorithm>
#include <cassert>

bool rts::ProductionQueue::add(World& w, UnitTypeId type) {
  auto& s{w[side_]};
  if (size() < MaxProductionQueueSize) {
    const auto& t{w[type]};
    if (auto [result, failedResource] =
            s.resources().allocateTo(resources_, t.cost, AllocFilter::Unrecoverable);
        result == AllocResult::Success) {
      queue_.push(type);
      return true;
    }
    else {
      auto& ui{w[failedResource].ui()};
      s.messages().add(w, result == AllocResult::Lack ? ui.msgMoreRequired() : ui.msgCapReached());
    }
  }
  else {
    s.messages().add(w, "THE QUEUE IS FULL!");
  }
  return false;
}

void rts::ProductionQueue::onDestroy(World& w) {
  w[side_].resources().deallocateFrom(resources_);
}

bool rts::ProductionQueue::startProduction(World& w, bool retrying) {
  auto& s{w[side_]};
  const auto& t{w[top()]};
  if (auto [result, failedResource] =
          s.resources().allocateTo(resources_, t.cost, AllocFilter::Recoverable);
      result == AllocResult::Success) {
    return true;
  }
  else if (!retrying) {
    auto& ui{w[failedResource].ui()};
    s.messages().add(w, result == AllocResult::Lack ? ui.msgMoreRequired() : ui.msgCapReached());
  }
  return false;
}

bool rts::ProductionQueue::finishProduction(World& w, const Unit& parent) {
  assert(!empty());
  auto towards{rallyPoint_ ? *rallyPoint_ : w.map.center()};
  if (auto p{w.emptyCellAround(parent.area, towards)}) {
    create(w, top(), *p);
    queue_.pop();
    assert(!empty() || std::all_of(resources_.begin(), resources_.end(), [](const auto& b) {
      return b.empty();
    }));
    return true;
  }
  else {
    return false;
  }
}

rts::GameTime rts::ProductionQueue::buildTime(const World& w) const {
  assert(!empty());
  return w[top()].buildTime;
}

void rts::ProductionQueue::create(World& w, UnitTypeId type, Point p) {
  w[side_].resources().restoreFrom(resources_, w[type].cost);
  // allocCheck = false; negative free slots are possible
  auto& unit{w[w.add(w.createUnit(type, side_), p, false)]};
  if (rallyPoint_) {
    const auto& unitType{w.unitTypes[unit.type]};
    auto target{w.abilityTarget(*rallyPoint_)};
    auto rc{
        std::holds_alternative<ResourceFieldId>(target) ? RelativeContent::Resource
                                                        : RelativeContent::Ground};
    if (auto ai{unitType.defaultAbility[uint32_t(rc)]}; ai != AbilityInstanceIndex::None)
      unit.trigger(ai, w, target);
  }
}
