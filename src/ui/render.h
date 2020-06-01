#pragma once

#include "rts/World.h"
#include "ui/Camera.h"

#include <ncursesw/ncurses.h>

namespace ui {
  void grid(WINDOW* win);
  void render(WINDOW* win, const rts::World& world, const Camera& camera);
}
