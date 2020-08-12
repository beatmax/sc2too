#pragma once

#include "Menu.h"
#include "rts/types.h"
#include "types.h"

namespace ui {
  struct IOState {
    Menu menu;

    Window headerWin;
    Window renderWin;
    Window controlWin;

    bool quit{false};
    rts::Point clickedTarget{-1, -1};
    rts::Point mousePosition{-1, -1};
    unsigned mouseButtons{0};
  };
}
