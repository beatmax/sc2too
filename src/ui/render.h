#pragma once

#include "rts/World.h"
#include "ui/Camera.h"

#include <ncurses.h>

namespace ui {
  void grid(WINDOW* win);
  void render(WINDOW* win, const rts::World& world, const Camera& camera);
}
