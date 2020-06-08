#pragma once

#include "AbilityState.h"

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace rts {
  class Ability {
  public:
    using CreateState = std::function<AbilityStateUPtr(Point)>;

    Ability(std::string name, CreateState cs)
      : name_{std::move(name)}, createState_{std::move(cs)} {}

    const std::string& name() const { return name_; }
    bool active() const { return bool(state_); }
    GameTime nextStepTime() const { return nextStepTime_; }

    void trigger(Point target) { state_ = createState_(target); }
    void step(const World& world, const EntitySPtr& entity, WorldActionList& actions);
    void cancel() {
      state_.reset();
      nextStepTime_ = 0;
    }

  private:
    const std::string name_;
    GameTime nextStepTime_{};
    const CreateState createState_;
    AbilityStateUPtr state_;
  };

  using AbilityList = std::vector<Ability>;
}
