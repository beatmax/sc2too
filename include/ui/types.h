#pragma once

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

#include "util/geo.h"

namespace ui {
  class Sprite;

  using ScreenPoint = util::geo::Point;
  using ScreenVector = util::geo::Vector;
  using ScreenRect = util::geo::Rectangle;

  struct Window {
    WINDOW* w{};
    int maxY{}, maxX{};
    int beginY{}, beginX{};
  };

  using ScrollDirection = unsigned int;
  constexpr ScrollDirection ScrollDirectionLeft{0b0001};
  constexpr ScrollDirection ScrollDirectionRight{0b0010};
  constexpr ScrollDirection ScrollDirectionUp{0b0100};
  constexpr ScrollDirection ScrollDirectionDown{0b1000};
}
