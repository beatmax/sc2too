#include "rts/ProductionQueue.h"

#include "rts/World.h"

#include <algorithm>
#include <cassert>

bool rts::ProductionQueue::add(World& w, UnitTypeId type) {
  auto& s{w[side_]};
  if (size() < MaxProductionQueueSize) {
    const auto& t{w[type]};
    if (auto [ok, lackingResource] = s.resources().tryTransferTo(resources_, t.cost); ok) {
      queue_.push(type);
      return true;
    }
    else {
      s.messages().add(w, lackingResource->ui().msgMoreRequired());
    }
  }
  else {
    s.messages().add(w, "THE QUEUE IS FULL!");
  }
  return false;
}

bool rts::ProductionQueue::startProduction(World& w) {
  return true;
}

bool rts::ProductionQueue::finishProduction(World& w, const Unit& parent) {
  assert(!empty());
  if (auto p{w.emptyCellAround(parent.area)}) {
    create(w, top(), *p);
    queue_.pop();
    assert(!empty() || resources_.depleted());
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
  const auto& t{w[type]};
  ResourceBank tmpBank;
  bool transferOk{resources_.tryTransferTo(tmpBank, t.cost).first};
  assert(transferOk);
  w.factory->create(w, type, p, side_);
}
