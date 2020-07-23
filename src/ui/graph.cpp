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

    void drawBordersNoBottom(WINDOW* win, int ulChar, int urChar) {
      int begY, begX, maxY, maxX;
      getbegyx(win, begY, begX);
      getmaxyx(win, maxY, maxX);

      mvaddch(begY - 1, begX - 1, ulChar);
      hline(0, maxX);
      mvaddch(begY - 1, begX + maxX, urChar);
      mvvline(begY, begX - 1, 0, maxY);
      mvvline(begY, begX + maxX, 0, maxY);
    }

    void drawBottomBorder(WINDOW* win) {
      int begY, begX, maxY, maxX;
      getbegyx(win, begY, begX);
      getmaxyx(win, maxY, maxX);

      mvaddch(begY + maxY, begX - 1, ACS_LLCORNER);
      hline(0, maxX);
      mvaddch(begY + maxY, begX + maxX, ACS_LRCORNER);
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

  raw();
  noecho();
  curs_set(0);

  initColors();
}

void ui::graph::drawBorders(const std::vector<WINDOW*>& windows) {
  attrset(white() | A_BOLD);
  auto it = windows.begin();
  if (it == windows.end())
    return;
  drawBordersNoBottom(*it++, ACS_ULCORNER, ACS_URCORNER);
  while (it != windows.end())
    drawBordersNoBottom(*it++, ACS_LTEE, ACS_RTEE);
  drawBottomBorder(windows.back());
}

void ui::graph::drawBoxSegment(WINDOW* win, int y, int x, BoxSegment segment) {
  wchar_t wch[2];
  if (mvwinnwstr(win, y, x, wch, 1) != ERR)
    segment = BoxSegment(int(segment) | int(wcharToSegment(wch[0])));
  wch[0] = segmentToWchar(segment);
  waddnwstr(win, wch, 1);
}
