#pragma once

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

#include "util/geo.h"

#include <cstdint>

namespace ui {
  class Frame;
  class Sprite;

  using FrameIndex = uint32_t;
  using ScreenPoint = util::geo::Point;
  using ScreenVector = util::geo::Vector;
  using ScreenRect = util::geo::Rectangle;

  struct Window {
    WINDOW* w{};
    int maxY{}, maxX{};
    int beginY{}, beginX{};
  };

  enum class Color : int {
    Red = 1,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    LightGreen = 10,
    DarkGreen = 22,
    DarkGrey = 237,
    Default = -1
  };

  using ScrollDirection = uint32_t;
  constexpr ScrollDirection ScrollDirectionLeft{0b0001};
  constexpr ScrollDirection ScrollDirectionRight{0b0010};
  constexpr ScrollDirection ScrollDirectionUp{0b0100};
  constexpr ScrollDirection ScrollDirectionDown{0b1000};
}
