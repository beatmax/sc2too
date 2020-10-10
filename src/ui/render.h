#pragma once

#include "rts/constants.h"
#include "rts/types.h"
#include "ui/Camera.h"
#include "ui/types.h"

namespace ui {
  void grid(const Window& win);
  void render(
      const Window& win,
      const rts::World& w,
      const rts::Map& m,
      const Camera& camera,
      const rts::Selection& selection);
  void render(
      const Window& win, const rts::World& w, const Camera& camera, const rts::WorldObject& object);
  void render(
      const Window& win,
      const rts::World& w,
      const Camera& camera,
      const rts::WorldObject& object,
      rts::Point center);
  void highlight(const Window& win, const Camera& camera, rts::Point cell, int color);
  void highlight(const Window& win, const Camera& camera, const rts::Rectangle& area, int color);
  void drawBoundingBox(
      const Window& win, const Camera& camera, const rts::Rectangle& area, int color);

#ifdef MAP_DEBUG
  void mapDebug(const Window& win, const rts::World& w, const Camera& camera);
#endif
}
