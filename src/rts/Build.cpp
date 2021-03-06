#include "Build.h"

#include "rts/World.h"

#include <cassert>

namespace rts {
  namespace {
    constexpr GameTime BuildDelay{10};
    constexpr GameTime MonitorTime{10};
    constexpr GameTime RetryTime{100};
    constexpr int MaxRetries{3};
  }
}

rts::ActiveAbilityStateUPtr rts::abilities::state::Build::trigger(
    World& w, Unit& u, ActiveAbilityStateUPtr& as, const Desc& desc, UnitId prototype) {
  assert(prototype);
  return std::make_unique<Build>(desc, prototype);
}

rts::AbilityStepResult rts::abilities::state::Build::step(const World& w, UnitStableRef u) {
  switch (state_) {
    case State::Init:
      return init(w, u);

    case State::MovingToTarget:
      if (Unit::abilityState(u, w, Kind::Move).active())
        return MonitorTime;
      if (adjacent(u->area, w[prototype_].area)) {
        state_ = State::Building;
        return GameTime{BuildDelay};
      }
      return [this](World& w, Unit& u) {
        w[u.side].messages().add(w, "IT'S BLOCKED!");
        return GameTime{0};
      };

    case State::Building:
      return build();
  }
  return GameTime{0};
}

void rts::abilities::state::Build::cleanup(World& w) {
  if (prototype_)
    w.destroy(prototype_);
}

rts::AbilityStepResult rts::abilities::state::Build::init(const World& w, const Unit& unit) {
  state_ = State::MovingToTarget;
  return [target{prototype_}](World& w, Unit& u) {
    auto moveIndex{w[u.type].abilityIndex(Kind::Move)};
    assert(moveIndex != AbilityStateIndex::None);
    u.trigger(moveIndex, w, target, {}, Unit::CancelOthers::No);
    return MonitorTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Build::build() {
  return [this](World& w, Unit& u) {
    if (w[prototype_].tryStartBuilding(w)) {
      prototype_ = {};
      return GameTime{0};
    }
    else if (++retryCount_ > MaxRetries) {
      w[u.side].messages().add(w, "I CAN'T BUILD HERE!");
      return GameTime{0};
    }
    return RetryTime;
  };
}
