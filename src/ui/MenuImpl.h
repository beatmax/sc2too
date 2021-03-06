#pragma once

#include "Menu.h"
#include "graph.h"

namespace ui {
  struct MenuImpl {
    static void print(const Menu& menu, const Window& win);
    static void processInput(Menu& menu, bool& quit);
  };
}
