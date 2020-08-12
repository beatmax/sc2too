#include "graph.h"

#include <cassert>

namespace ui::graph {
  namespace {
    void initColors() {
      if (has_colors()) {
        start_color();
        use_default_colors();

        for (int i = 1; i < COLORS; ++i) {
          assert(i < COLOR_PAIRS);
          init_pair(i, i, -1);
        }
      }
    }

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

void ui::graph::init() {
  initscr();

  noecho();
  curs_set(0);

  initColors();
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
      graph::drawBoxSegment(win, p.y, x, graph::BoxSegment::HLine);
  }
}

void ui::graph::drawVLine(const Window& win, ScreenPoint p, int height) {
  if (p.x >= 0 && p.x < win.maxX) {
    auto begin{std::max(0, int(p.y))};
    auto end{std::min(int(p.y + height), win.maxY)};
    for (auto y{begin}; y < end; ++y)
      graph::drawBoxSegment(win, y, p.x, graph::BoxSegment::VLine);
  }
}

void ui::graph::drawRect(const Window& win, const ScreenRect& rect) {
  const ScreenPoint bottomRight{rect.topLeft + rect.size - ScreenVector{1, 1}};
  const auto& [xLeft, yTop] = rect.topLeft;
  const auto& [xRight, yBottom] = bottomRight;
  if (yTop >= 0) {
    if (xLeft >= 0)
      graph::drawBoxSegment(win, yTop, xLeft, graph::BoxSegment::ULCorner);
    if (xRight < win.maxX)
      graph::drawBoxSegment(win, yTop, xRight, graph::BoxSegment::URCorner);
  }
  if (yBottom < win.maxY) {
    if (xLeft >= 0)
      graph::drawBoxSegment(win, yBottom, xLeft, graph::BoxSegment::LLCorner);
    if (xRight < win.maxX)
      graph::drawBoxSegment(win, yBottom, xRight, graph::BoxSegment::LRCorner);
  }
  drawHLine(win, {xLeft + 1, yTop}, rect.size.x - 2);
  drawHLine(win, {xLeft + 1, yBottom}, rect.size.x - 2);
  drawVLine(win, {xLeft, yTop + 1}, rect.size.y - 2);
  drawVLine(win, {xRight, yTop + 1}, rect.size.y - 2);
}
