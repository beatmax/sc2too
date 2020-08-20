#include "Produce.h"

#include "rts/Entity.h"
#include "rts/ProductionQueue.h"
#include "rts/World.h"

#include <cassert>

namespace rts {
  namespace {
    constexpr GameTime MonitorTime{10};
  }
}

void rts::abilities::state::Produce::trigger(
    World& w, Entity& e, ActiveAbilityStateUPtr& as, const Desc& desc, Point) {
  assert(e.productionQueue);
  auto& pq{w[e.productionQueue]};

  assert(pq.size() == 0 || as);

  if (pq.add(w, desc.type) && !as)
    as = std::make_unique<Produce>();
}

rts::AbilityStepResult rts::abilities::state::Produce::step(const World& w, const Entity& e) {
  auto& pq{w[e.productionQueue]};
  if (pq.empty())
    return GameTime{0};

  switch (state_) {
    case State::Idle:
      return startProduction();
    case State::Producing:
      return finishProduction();
  }
  return GameTime{0};
}

rts::AbilityStepAction rts::abilities::state::Produce::startProduction() {
  return [this](World& w, Entity& e) -> GameTime {
    auto& pq{w[e.productionQueue]};
    if (pq.empty())
      return 0;
    if (pq.startProduction(w)) {
      state_ = State::Producing;
      return pq.buildTime(w);
    }
    return MonitorTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Produce::finishProduction() {
  return [this](World& w, Entity& e) -> GameTime {
    auto& pq{w[e.productionQueue]};
    if (pq.finishProduction(w, e)) {
      state_ = State::Idle;
      return 1;
    }
    return MonitorTime;
  };
}
