#pragma once

#include "graph.h"
#include "rts/constants.h"
#include "rts/types.h"
#include "ui/Camera.h"

namespace ui {
  void grid(WINDOW* win);
  void render(
      WINDOW* win, const rts::World& w, const Camera& camera, const rts::Selection& selection);
  void highlight(WINDOW* win, const Camera& camera, rts::Point cell, int color);
  void drawBoundingBox(WINDOW* win, const Camera& camera, const rts::Rectangle& area, int color);

#ifdef MAP_DEBUG
  void mapDebug(WINDOW* win, const rts::World& w, const Camera& camera);
#endif
}
