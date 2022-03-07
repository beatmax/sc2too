#include "rts/ProductionQueue.h"

#include "rts/World.h"

#include <algorithm>
#include <cassert>

bool rts::ProductionQueue::add(World& w, ProducibleId product) {
  auto& s{w[side_]};
  if (size() < MaxProductionQueueSize) {
    const auto& p{w[product]};
    if (auto [result, failedResource] =
            s.resources().allocateTo(resources_, p.cost, AllocFilter::Unrecoverable);
        result == AllocResult::Success) {
      queue_.push(product);
      if (auto* u{std::get_if<UpgradeId>(&product)})
        s.setUpgradeInResearch(*u, true);
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
  auto& s{w[side_]};
  s.resources().deallocateFrom(resources_);
  for (size_t i{0}; i < size(); ++i) {
    auto p{product(i)};
    if (auto* u{std::get_if<UpgradeId>(&p)})
      s.setUpgradeInResearch(*u, false);
  }
}

bool rts::ProductionQueue::startProduction(World& w, bool retrying) {
  auto& s{w[side_]};
  const auto& p{w[top()]};
  if (auto [result, failedResource] =
          s.resources().allocateTo(resources_, p.cost, AllocFilter::Recoverable);
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
  auto product{top()};
  if (auto* type{std::get_if<UnitTypeId>(&product)}) {
    auto towards{rallyPoint_ ? *rallyPoint_ : w.map.center()};
    if (auto p{w.emptyCellAround(parent.area, towards)}) {
      create(w, *type, *p);
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
  else {
    auto& s{w[side_]};
    s.messages().add(w, "RESEARCH COMPLETE!");
    s.addUpgrade(std::get<UpgradeId>(product));
    queue_.pop();
    return true;
  }
}

rts::GameTime rts::ProductionQueue::produceTime(const World& w) const {
  assert(!empty());
  return w[top()].produceTime;
}

void rts::ProductionQueue::boost(const World& w, Percent speedUp, GameTime duration) {
  boostUntil_ = w.time + duration;
  boostWorkPerCycle_ = (ProduceCycleTime * (100 + speedUp)) / 100;
}

bool rts::ProductionQueue::boosted(const World& w) const {
  return w.time < boostUntil_;
}

rts::GameTime rts::ProductionQueue::workPerCycle(const World& w) const {
  return boosted(w) ? boostWorkPerCycle_ : ProduceCycleTime;
}

void rts::ProductionQueue::create(World& w, UnitTypeId type, Point p) {
  w[side_].resources().restoreFrom(resources_, w[type].cost);
  // allocCheck = false; negative free slots are possible
  auto& unit{w[w.add(w.createUnit(type, side_), p, false)]};
  if (rallyPoint_) {
    auto rc{w.relativeContentForRally(side_, *rallyPoint_)};
    const auto& unitType{w.unitTypes[unit.type]};
    if (auto ai{unitType.defaultAbility[uint32_t(rc)]}; ai != AbilityInstanceIndex::None)
      unit.trigger(ai, w, w.abilityTarget(*rallyPoint_));
  }
}
