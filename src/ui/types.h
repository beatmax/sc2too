#pragma once

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

#include "util/geo.h"

namespace ui {
  using ScreenPoint = util::geo::Point;
  using ScreenVector = util::geo::Vector;
  using ScreenRect = util::geo::Rectangle;

  struct Window {
    WINDOW* w{};
    int maxY{}, maxX{};
    int beginY{}, beginX{};
  };
}
