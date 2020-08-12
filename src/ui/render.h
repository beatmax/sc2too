#pragma once

#include "rts/constants.h"
#include "rts/types.h"
#include "types.h"
#include "ui/Camera.h"

namespace ui {
  void grid(const Window& win);
  void render(
      const Window& win,
      const rts::World& w,
      const Camera& camera,
      const rts::Selection& selection);
  void highlight(const Window& win, const Camera& camera, rts::Point cell, int color);
  void drawBoundingBox(
      const Window& win, const Camera& camera, const rts::Rectangle& area, int color);

#ifdef MAP_DEBUG
  void mapDebug(const Window& win, const rts::World& w, const Camera& camera);
#endif
}
