#pragma once

#include "graph.h"
#include "ui/Menu.h"

namespace ui {
  struct MenuImpl {
    static void print(const Menu& menu, WINDOW* win);
    static void processInput(Menu& menu, bool& quit);
  };
}
