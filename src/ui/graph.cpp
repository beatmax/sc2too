#include "graph.h"

#include <cassert>

namespace ui::graph {
  namespace {
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

  }
}

void ui::graph::init() {
  initscr();

  raw();
  noecho();
  keypad(stdscr, true);
  curs_set(0);

  if (has_colors()) {
    start_color();
    use_default_colors();

    for (int i = 1; i < COLORS; ++i) {
      assert(i < COLOR_PAIRS);
      init_pair(i, i, -1);
    }
  }
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
