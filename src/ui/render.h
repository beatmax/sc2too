#pragma once

#include "graph.h"
#include "rts/World.h"
#include "ui/Camera.h"

namespace ui {
  void grid(WINDOW* win);
  void highlight(WINDOW* win, const Camera& camera, rts::Point cell, int color);
  void render(WINDOW* win, const rts::World& world, const Camera& camera);

#ifdef MAP_DEBUG
  void mapDebug(WINDOW* win, const rts::World& world, const Camera& camera);
#endif
}
