#pragma once

#include "ui/types.h"

#include <vector>

namespace ui::graph {
  int colorPair(int fg, int bg);
  inline int colorPair(Color fg, Color bg = Color::Default) { return colorPair(int(fg), int(bg)); }
  inline int colorAttr(Color fg, Color bg = Color::Default) {
    return COLOR_PAIR(colorPair(fg, bg));
  }

  inline int red() { return colorAttr(Color::Red); }
  inline int green() { return colorAttr(Color::Green); }
  inline int yellow() { return colorAttr(Color::Yellow); }
  inline int blue() { return colorAttr(Color::Blue); }
  inline int magenta() { return colorAttr(Color::Magenta); }
  inline int cyan() { return colorAttr(Color::Cyan); }
  inline int white() { return colorAttr(Color::White); }
  inline int gray() { return colorAttr(Color::Gray); }
  inline int brightRed() { return colorAttr(Color::BrightRed); }
  inline int brightGreen() { return colorAttr(Color::BrightGreen); }
  inline int brightYellow() { return colorAttr(Color::BrightYellow); }
  inline int brightBlue() { return colorAttr(Color::BrightBlue); }
  inline int brightMagenta() { return colorAttr(Color::BrightMagenta); }
  inline int brightCyan() { return colorAttr(Color::BrightCyan); }
  inline int brightWhite() { return colorAttr(Color::BrightWhite); }
  inline int black() { return colorAttr(Color::Black); }
  inline int darkBlue() { return colorAttr(Color::DarkBlue); }
  inline int darkGreen() { return colorAttr(Color::DarkGreen); }
  inline int darkRed() { return colorAttr(Color::DarkRed); }
  inline int darkGray() { return colorAttr(Color::DarkGray); }
  inline int electricBlue1() { return colorAttr(Color::ElectricBlue1); }
  inline int electricBlue2() { return colorAttr(Color::ElectricBlue2); }

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
  ScreenVector screenSize();
  void drawBorders(const std::vector<const Window*>& windows);
  void drawBoxSegment(const Window& win, int y, int x, BoxSegment segment);
  void drawHLine(const Window& win, ScreenPoint p, int width);
  void drawVLine(const Window& win, ScreenPoint p, int height);
  void drawRect(const Window& win, const ScreenRect& rect);
  void drawRectNoCorners(const Window& win, const ScreenRect& rect);
  void drawFrame(
      const Window& win,
      const Frame& frame,
      const ScreenRect& drawRect,
      const ScreenVector& topLeftInFrame,
      Color defaultColor = Color::White);
  void drawFrame(
      const Window& win,
      const Frame& frame,
      ScreenPoint screenTopLeft,
      Color defaultColor = Color::White);
}
