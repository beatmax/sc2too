
#pragma once

#include "Camera.h"
#include "InputEvent.h"
#include "rts/Selection.h"
#include "rts/Side.h"
#include "rts/WorldAction.h"
#include "rts/types.h"

namespace ui {
  struct Player {
    rts::SideId side;
    Camera camera;
    rts::Selection selection;

    rts::WorldActionList processInput(const rts::World& world, const InputEvent& event);
  };
}
