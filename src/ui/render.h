#pragma once

#include "graph.h"
#include "rts/World.h"
#include "ui/Camera.h"

namespace ui {
  void grid(WINDOW* win);
  void render(WINDOW* win, const rts::World& world, const Camera& camera);
}
