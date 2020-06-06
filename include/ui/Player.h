
#pragma once

#include "Camera.h"
#include "rts/Side.h"

namespace ui {
  struct Player {
    const rts::Side& side;
    Camera camera;
  };
}
