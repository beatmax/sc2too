#pragma once

#include "ui/types.h"

#include <vector>

namespace ui::graph {
  enum class ColorPair : int {
    Red = 1,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    LightGreen = 10,
    DarkGreen = 22,
    DarkGrey = 237
  };

  inline int red() { return COLOR_PAIR(ColorPair::Red); }
  inline int green() { return COLOR_PAIR(ColorPair::Green); }
  inline int yellow() { return COLOR_PAIR(ColorPair::Yellow); }
  inline int blue() { return COLOR_PAIR(ColorPair::Blue); }
  inline int magenta() { return COLOR_PAIR(ColorPair::Magenta); }
  inline int cyan() { return COLOR_PAIR(ColorPair::Cyan); }
  inline int white() { return COLOR_PAIR(ColorPair::White); }
  inline int lightGreen() { return COLOR_PAIR(ColorPair::LightGreen); }
  inline int darkGreen() { return COLOR_PAIR(ColorPair::DarkGreen); }
  inline int darkGrey() { return COLOR_PAIR(ColorPair::DarkGrey); }

  enum class BoxSegment : int {
    None = 0b0000,
    ULCorner = 0b0110,
    LLCorner = 0b1100,
    URCorner = 0b0011,
    LRCorner = 0b1001,
    LTee = 0b1110,
    RTee = 0b1011,
    BTee = 0b1101,
    TTee = 0b0111,
    HLine = 0b0101,
    VLine = 0b1010,
    Plus = 0b1111
  };

  void init();
  void drawBorders(const std::vector<const Window*>& windows);
  void drawBoxSegment(const Window& win, int y, int x, BoxSegment segment);
  void drawHLine(const Window& win, ScreenPoint p, int width);
  void drawVLine(const Window& win, ScreenPoint p, int height);
  void drawRect(const Window& win, const ScreenRect& rect);
  void drawFrame(
      const Window& win,
      const Frame& frame,
      const ScreenRect& drawRect,
      const ScreenVector& topLeftInFrame);
}
