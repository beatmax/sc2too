#include "rts/ProductionQueue.h"

#include "rts/World.h"

#include <algorithm>
#include <cassert>

bool rts::ProductionQueue::add(World& w, EntityTypeId type) {
  auto& s{w[side_]};
  if (size_ < MaxProductionQueueSize) {
    const auto& t{w[type]};
    if (auto [ok, lackingResource] = s.resources().tryTransferTo(resources_, t.cost); ok) {
      array_[size_++] = type;
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

bool rts::ProductionQueue::finishProduction(World& w, const Entity& parent) {
  assert(!empty());
  if (auto p{w.emptyCellAround(parent.area)}) {
    create(w, top(), *p);
    pop();
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

void rts::ProductionQueue::create(World& w, EntityTypeId type, Point p) {
  const auto& t{w[type]};
  ResourceBank tmpBank;
  bool transferOk{resources_.tryTransferTo(tmpBank, t.cost).first};
  assert(transferOk);
  w.factory->create(w, type, p, side_);
}

void rts::ProductionQueue::pop() {
  assert(size_ > 0);
  std::move(array_.begin() + 1, array_.begin() + size_, array_.begin());
  --size_;
}
