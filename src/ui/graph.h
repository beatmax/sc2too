#pragma once

#include <ncursesw/ncurses.h>
#include <vector>

namespace ui::graph {
  enum class ColorPair : int { Red = 1, Green, Yellow, Blue, Magenta, Cyan, White, DarkGrey };

  inline int red() { return COLOR_PAIR(ColorPair::Red); }
  inline int green() { return COLOR_PAIR(ColorPair::Green); }
  inline int yellow() { return COLOR_PAIR(ColorPair::Yellow); }
  inline int blue() { return COLOR_PAIR(ColorPair::Blue); }
  inline int magenta() { return COLOR_PAIR(ColorPair::Magenta); }
  inline int cyan() { return COLOR_PAIR(ColorPair::Cyan); }
  inline int white() { return COLOR_PAIR(ColorPair::White); }
  inline int darkGrey() { return COLOR_PAIR(ColorPair::DarkGrey); }

  void init();
  void drawBorders(const std::vector<WINDOW*>& windows);
}
