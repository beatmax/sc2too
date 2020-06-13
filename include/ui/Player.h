
#pragma once

#include "Camera.h"
#include "InputEvent.h"
#include "rts/Side.h"

namespace ui {
  struct Player {
    rts::SideCPtr side;
    Camera camera;

    void processInput(const InputEvent& event);
  };
}
