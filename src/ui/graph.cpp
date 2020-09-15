#include "graph.h"

#include "Frame.h"

#include <cassert>
#include <stdexcept>
#include <unordered_map>

namespace ui::graph {
  namespace {
    struct PairHash {
      template<class T1, class T2>
      size_t operator()(const std::pair<T1, T2>& p) const {
        return (std::hash<T1>()(p.first) << 8) ^ std::hash<T2>()(p.second);
      }
    };

    std::unordered_map<std::pair<int, int>, int, PairHash> colorPairs;
    int nextColorPair = 1;

    void drawBordersNoBottom(const Window& win, int ulChar, int urChar) {
      mvaddch(win.beginY - 1, win.beginX - 1, ulChar);
      hline(0, win.maxX);
      mvaddch(win.beginY - 1, win.beginX + win.maxX, urChar);
      mvvline(win.beginY, win.beginX - 1, 0, win.maxY);
      mvvline(win.beginY, win.beginX + win.maxX, 0, win.maxY);
    }

    void drawBottomBorder(const Window& win) {
      mvaddch(win.beginY + win.maxY, win.beginX - 1, ACS_LLCORNER);
      hline(0, win.maxX);
      mvaddch(win.beginY + win.maxY, win.beginX + win.maxX, ACS_LRCORNER);
    }

    BoxSegment wcharToSegment(wchar_t wch) {
      switch (wch) {
        case L'╭':
          return BoxSegment::ULCorner;
        case L'╰':
          return BoxSegment::LLCorner;
        case L'╮':
          return BoxSegment::URCorner;
        case L'╯':
          return BoxSegment::LRCorner;
        case L'├':
          return BoxSegment::LTee;
        case L'┤':
          return BoxSegment::RTee;
        case L'┴':
          return BoxSegment::BTee;
        case L'┬':
          return BoxSegment::TTee;
        case L'─':
          return BoxSegment::HLine;
        case L'│':
          return BoxSegment::VLine;
        case L'┼':
          return BoxSegment::Plus;
        default:
          return BoxSegment::None;
      }
    }

    wchar_t segmentToWchar(BoxSegment segment) {
      switch (segment) {
        case BoxSegment::None:
          return L' ';
        case BoxSegment::ULCorner:
          return L'╭';
        case BoxSegment::LLCorner:
          return L'╰';
        case BoxSegment::URCorner:
          return L'╮';
        case BoxSegment::LRCorner:
          return L'╯';
        case BoxSegment::LTee:
          return L'├';
        case BoxSegment::RTee:
          return L'┤';
        case BoxSegment::BTee:
          return L'┴';
        case BoxSegment::TTee:
          return L'┬';
        case BoxSegment::HLine:
          return L'─';
        case BoxSegment::VLine:
          return L'│';
        case BoxSegment::Plus:
          return L'┼';
      }
      return L'?';
    }
  }
}

int ui::graph::colorPair(int fg, int bg) {
  auto it{colorPairs.find({fg, bg})};
  if (it != colorPairs.end())
    return it->second;

  assert(nextColorPair < COLOR_PAIRS);
  init_pair(nextColorPair, fg, bg);
  colorPairs.emplace(std::make_pair(fg, bg), nextColorPair);
  return nextColorPair++;
}

void ui::graph::init() {
  initscr();

  if (!has_colors())
    throw std::runtime_error{"terminal does not support colors"};

  noecho();
  curs_set(0);
  start_color();
  use_default_colors();

  colorPairs.emplace(std::make_pair(0, int(Color::Default)), 0);
}

void ui::graph::drawBorders(const std::vector<const Window*>& windows) {
  attrset(white() | A_BOLD);
  auto it = windows.begin();
  if (it == windows.end())
    return;
  drawBordersNoBottom(**it++, ACS_ULCORNER, ACS_URCORNER);
  while (it != windows.end())
    drawBordersNoBottom(**it++, ACS_LTEE, ACS_RTEE);
  drawBottomBorder(*windows.back());
}

void ui::graph::drawBoxSegment(const Window& win, int y, int x, BoxSegment segment) {
  wchar_t wch[2];
  if (mvwinnwstr(win.w, y, x, wch, 1) != ERR)
    segment = BoxSegment(int(segment) | int(wcharToSegment(wch[0])));
  wch[0] = segmentToWchar(segment);
  waddnwstr(win.w, wch, 1);
}

void ui::graph::drawHLine(const Window& win, ScreenPoint p, int width) {
  if (p.y >= 0 && p.y < win.maxY) {
    auto begin{std::max(0, int(p.x))};
    auto end{std::min(int(p.x + width), win.maxX)};
    for (auto x{begin}; x < end; ++x)
      drawBoxSegment(win, p.y, x, BoxSegment::HLine);
  }
}

void ui::graph::drawVLine(const Window& win, ScreenPoint p, int height) {
  if (p.x >= 0 && p.x < win.maxX) {
    auto begin{std::max(0, int(p.y))};
    auto end{std::min(int(p.y + height), win.maxY)};
    for (auto y{begin}; y < end; ++y)
      drawBoxSegment(win, y, p.x, BoxSegment::VLine);
  }
}

void ui::graph::drawRect(const Window& win, const ScreenRect& rect) {
  const auto& [xLeft, yTop] = rect.topLeft;
  const auto& [xRight, yBottom] = rect.bottomRight();
  if (yTop >= 0) {
    if (xLeft >= 0)
      drawBoxSegment(win, yTop, xLeft, BoxSegment::ULCorner);
    if (xRight < win.maxX)
      drawBoxSegment(win, yTop, xRight, BoxSegment::URCorner);
  }
  if (yBottom < win.maxY) {
    if (xLeft >= 0)
      drawBoxSegment(win, yBottom, xLeft, BoxSegment::LLCorner);
    if (xRight < win.maxX)
      drawBoxSegment(win, yBottom, xRight, BoxSegment::LRCorner);
  }
  drawHLine(win, {xLeft + 1, yTop}, rect.size.x - 2);
  drawHLine(win, {xLeft + 1, yBottom}, rect.size.x - 2);
  drawVLine(win, {xLeft, yTop + 1}, rect.size.y - 2);
  drawVLine(win, {xRight, yTop + 1}, rect.size.y - 2);
}

void ui::graph::drawRectNoCorners(const Window& win, const ScreenRect& rect) {
  const auto& [xLeft, yTop] = rect.topLeft;
  const auto& [xRight, yBottom] = rect.bottomRight();
  drawHLine(win, {xLeft + 1, yTop}, rect.size.x - 2);
  drawHLine(win, {xLeft + 1, yBottom}, rect.size.x - 2);
  drawVLine(win, {xLeft, yTop + 1}, rect.size.y - 2);
  drawVLine(win, {xRight, yTop + 1}, rect.size.y - 2);
}

void ui::graph::drawFrame(
    const Window& win,
    const Frame& frame,
    const ScreenRect& drawRect,
    const ScreenVector& topLeftInFrame) {
  ScreenVector v{0, 0};
  for (; v.y < drawRect.size.y; ++v.y) {
    const auto sp{topLeftInFrame + v};
    mvwadd_wchnstr(
        win.w, drawRect.topLeft.y + v.y, drawRect.topLeft.x, &frame(sp.y, sp.x), drawRect.size.x);
  }
}
