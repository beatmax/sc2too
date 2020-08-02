#pragma once

#include "Camera.h"
#include "InputEvent.h"
#include "rts/Command.h"
#include "rts/types.h"

#include <optional>

namespace ui {
  struct Player {
    rts::SideId side;
    Camera camera;

    std::optional<rts::Command> processInput(const rts::World& world, const InputEvent& event);

  private:
    rts::EntityIdList visibleSameType(const rts::World& world, rts::EntityId entity);
  };
}
