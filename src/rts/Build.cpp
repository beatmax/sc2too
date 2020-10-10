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

void rts::abilities::state::Build::trigger(
    World& w, Unit& u, ActiveAbilityStateUPtr& as, const Desc& desc, const AbilityTarget& target) {
  assert(std::holds_alternative<Point>(target));
  auto targetPoint{std::get<Point>(target)};

  auto& side{w[u.side]};
  assert(side.prototype());
  auto& proto{w[side.prototype()]};
  auto buildArea{rectangleCenteredAt(targetPoint, proto.area.size, w.map.area())};
  if (!w.objectsInArea(buildArea).empty()) {
    side.messages().add(w, "INVALID LOCATION!");
    return;
  }
  if (!proto.tryAllocate(w))
    return;
  proto.setBuildPoint(w, buildArea.topLeft);

  if (as)
    as->cancel(w);
  as = std::make_unique<Build>(desc, side.takePrototype());
}

rts::AbilityStepResult rts::abilities::state::Build::step(const World& w, UnitStableRef u) {
  switch (state_) {
    case State::Init:
      return init(w, u);

    case State::MovingToTarget:
      if (Unit::abilityState(u, w, Kind::Move).active())
        return MonitorTime;
      if (adjacent(u->area, w[builtUnit_].area)) {
        state_ = State::Building;
        return GameTime{BuildDelay};
      }
      return GameTime{0};

    case State::Building:
      return build();
  }
  return GameTime{0};
}

void rts::abilities::state::Build::cancel(World& w) {
  w.destroy(builtUnit_);
}

rts::AbilityStepResult rts::abilities::state::Build::init(const World& w, const Unit& unit) {
  state_ = State::MovingToTarget;
  return [target{builtUnit_}](World& w, Unit& u) {
    auto moveIndex{w[u.type].abilityIndex(Kind::Move)};
    assert(moveIndex != AbilityStateIndex::None);
    u.trigger(moveIndex, w, target, Unit::CancelOthers::No);
    return MonitorTime;
  };
}

rts::AbilityStepAction rts::abilities::state::Build::build() {
  return [this](World& w, Unit& u) {
    if (w[builtUnit_].tryStartBuilding(w)) {
      return GameTime{0};
    }
    else if (++retryCount_ > MaxRetries) {
      w[u.side].messages().add(w, "I CAN'T BUILD HERE!");
      w.destroy(builtUnit_);
      return GameTime{0};
    }
    return RetryTime;
  };
}
