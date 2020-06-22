
#pragma once

#include "Camera.h"
#include "InputEvent.h"
#include "rts/Side.h"
#include "rts/WorldAction.h"
#include "rts/types.h"

namespace ui {
  struct Player {
    rts::SideCPtr side;
    Camera camera;
    rts::EntityWPtr selection;

    rts::WorldActionList processInput(const rts::World& world, const InputEvent& event);
  };
}
