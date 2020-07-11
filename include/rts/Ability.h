#pragma once

#include "AbilityState.h"
#include "dimensions.h"
#include "types.h"

#include <functional>
#include <memory>
#include <string>
#include <utility>

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
    void step(const World& world, const Entity& entity, WorldActionList& actions);
    void stepAction(World& world, Entity& entity);
    void cancel();

  private:
    const std::string name_;
    GameTime nextStepTime_{GameTimeInf};
    const CreateState createState_;
    AbilityStateUPtr state_;
  };
}
