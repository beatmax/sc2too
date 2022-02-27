#pragma once

#include "ui/types.h"

#include <vector>

namespace ui::graph {
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

  short colorPair(short fg, short bg);
  inline short colorPair(Color fg, Color bg = Color::Default) {
    return colorPair(short(fg), short(bg));
  }
  void setColor(const Window& win, short pair);
  void setColor(const Window& win, Color fg, Color bg = Color::Default);

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
  void drawOverlay(
      const Window& win,
      const Frame& frame,
      const ScreenRect& drawRect,
      const ScreenVector& topLeftInFrame,
      Color defaultColor = Color::White);
}
