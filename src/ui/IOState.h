#pragma once

#include "Menu.h"
#include "rts/types.h"

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

namespace ui {
  struct IOState {
    Menu menu;

    WINDOW* headerWin{nullptr};
    WINDOW* renderWin{nullptr};
    WINDOW* controlWin{nullptr};

    bool quit{false};
    rts::Point clickedTarget{-1, -1};
    rts::Point mousePosition{-1, -1};
    unsigned mouseButtons{0};
  };
}
