
#pragma once

#include "Camera.h"
#include "rts/Side.h"

namespace ui {
  struct Player {
    rts::SideCPtr side;
    Camera camera;
  };
}
