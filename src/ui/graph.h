#pragma once

#include "ui/types.h"

#include <vector>

namespace ui::graph {
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

  int colorPair(int fg, int bg);
  inline int colorPair(Color fg, Color bg) { return colorPair(int(fg), int(bg)); }
  inline int red() { return COLOR_PAIR(colorPair(Color::Red, Color::Default)); }
  inline int green() { return COLOR_PAIR(colorPair(Color::Green, Color::Default)); }
  inline int yellow() { return COLOR_PAIR(colorPair(Color::Yellow, Color::Default)); }
  inline int blue() { return COLOR_PAIR(colorPair(Color::Blue, Color::Default)); }
  inline int magenta() { return COLOR_PAIR(colorPair(Color::Magenta, Color::Default)); }
  inline int cyan() { return COLOR_PAIR(colorPair(Color::Cyan, Color::Default)); }
  inline int white() { return COLOR_PAIR(colorPair(Color::White, Color::Default)); }
  inline int lightGreen() { return COLOR_PAIR(colorPair(Color::LightGreen, Color::Default)); }
  inline int darkGreen() { return COLOR_PAIR(colorPair(Color::DarkGreen, Color::Default)); }
  inline int darkGrey() { return COLOR_PAIR(colorPair(Color::DarkGrey, Color::Default)); }

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
