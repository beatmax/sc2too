#include "Produce.h"

#include "rts/ProductionQueue.h"
#include "rts/Unit.h"
#include "rts/World.h"

#include <cassert>

rts::ActiveAbilityStateUPtr rts::abilities::state::Produce::trigger(
    World& w, Unit& u, ActiveAbilityStateUPtr& as, const Desc& desc, const AbilityTarget&) {
  assert(u.productionQueue);
  auto& pq{w[u.productionQueue]};

  assert(pq.size() == 0 || as);

  if (pq.add(w, desc.type)) {
    if (!as)
      return std::make_unique<Produce>();
    else
      static_cast<Produce&>(*as).unblock();
  }

  return {};
}

rts::AbilityStepResult rts::abilities::state::Produce::step(const World& w, UnitStableRef u) {
  auto& pq{w[u->productionQueue]};
  if (pq.empty())
    return GameTime{0};

  switch (state_) {
    case State::Idle:
    case State::Blocked:
      return startProduction();
    case State::Producing:
      if (u->powered) {
        auto wps{pq.workPerCycle(w)};
        if (workLeft_ <= wps)
          return finishProduction();
        workLeft_ -= wps;
      }
      return ProduceCycleTime;
  }
  return GameTime{0};
}

void rts::abilities::state::Produce::unblock() {
  if (state_ == State::Blocked)
    state_ = State::Idle;
}

rts::AbilityStepAction rts::abilities::state::Produce::startProduction() {
  return [this](World& w, Unit& u) -> GameTime {
    auto& pq{w[u.productionQueue]};
    if (pq.empty())
      return 0;
    if (pq.startProduction(w, state_ == State::Blocked)) {
      workLeft_ = pq.buildTime(w);
      state_ = State::Producing;
    }
    else {
      state_ = State::Blocked;
    }
    return ProduceCycleTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Produce::finishProduction() {
  return [this](World& w, Unit& u) -> GameTime {
    auto& pq{w[u.productionQueue]};
    if (pq.finishProduction(w, u)) {
      state_ = State::Idle;
      return 1;
    }
    return ProduceCycleTime;
  };
}
